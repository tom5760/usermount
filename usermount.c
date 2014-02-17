/*
 * Copyright (c) 2014 Tom Wambold <tom5760@gmail.com>
 * Copyright (c) 2014 Marius L. Jøhndal <mariuslj@ifi.uio.no>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#define UDISKS_API_IS_SUBJECT_TO_CHANGE
#include <udisks/udisks.h>
#include <libnotify/notify.h>

static const char *BLOCK_PATH = "/org/freedesktop/UDisks2/block_devices/";
static const int NOTIFICATION_TIMEOUT_MS = 5000; /* 5 seconds */
static const char *NOTIFICATION_APP_NAME = "usermount";

static void send_notification(char *path) {
    NotifyNotification *n;
    gchar *message;
    GError *error = NULL;

    notify_init(NOTIFICATION_APP_NAME);

    message = g_strdup_printf("Device mounted on %s", path);

    n = notify_notification_new(NOTIFICATION_APP_NAME, message, NULL);
    notify_notification_set_timeout(n, NOTIFICATION_TIMEOUT_MS);

    if (!notify_notification_show(n, &error)) {
      fprintf(stderr, "Failed to send notification: %s\n", error->message);
      g_error_free(error);
    }

    g_free(message);
    g_object_unref(G_OBJECT(n));
}

static void on_object_added(GDBusObjectManager *manager,
                            GDBusObject *dbus_object, gpointer user_data) {
    UDisksObject *object = NULL;
    UDisksBlock *block = NULL;
    UDisksFilesystem *filesystem = NULL;

    const char *path = g_dbus_object_get_object_path(dbus_object);
    fprintf(stderr, "New object: %s: ", path);

    if (strncmp(path, BLOCK_PATH, strlen(BLOCK_PATH)) != 0) {
        fprintf(stderr, "Not a block device\n");
        return;
    }

    object = UDISKS_OBJECT(dbus_object);

    block = udisks_object_peek_block(object);
    if (block == NULL) {
        fprintf(stderr, "Not a block object\n");
        return;
    }

    filesystem = udisks_object_peek_filesystem(object);
    if (filesystem == NULL) {
        fprintf(stderr, "Not a mountable filesystem\n");
        return;
    }

    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_add(&builder, "{sv}", "auth.no_user_interaction",
                          g_variant_new_boolean(TRUE));

    GVariant *options = g_variant_builder_end (&builder);
    g_variant_ref_sink (options);

    gchar *mount_path = NULL;
    GError *error = NULL;
    if (!udisks_filesystem_call_mount_sync(
                filesystem, options, &mount_path, NULL, &error)) {
        fprintf(stderr, "Error mounting: %s\n", error->message);
        g_error_free(error);
    } else {
        fprintf(stderr, "Mounting device at: %s\n", mount_path);
        send_notification(mount_path);
        g_free(mount_path);
    }
    g_variant_unref(options);
}

static void on_interface_added(GDBusObjectManager *manager,
                               GDBusObject *dbus_object,
                               GDBusInterface *interface, gpointer user_data) {
    on_object_added(manager, dbus_object, user_data);
}

int main(int argc, char **argv) {
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    GError *error = NULL;
    UDisksClient *client = udisks_client_new_sync(NULL, &error);
    if (client == NULL) {
        fprintf(stderr, "Error connecting to the udisks daemon: %s\n",
                error->message);
        g_error_free(error);
        g_main_loop_unref(loop);
        return 1;
    }

    GDBusObjectManager *manager = udisks_client_get_object_manager(client);
    g_signal_connect(manager, "object-added", G_CALLBACK(on_object_added),
                     NULL);
    g_signal_connect(manager, "interface-added",
                     G_CALLBACK(on_interface_added), NULL);

    g_main_loop_run(loop);

    g_object_unref(client);
    g_main_loop_unref(loop);

    return 0;
}
