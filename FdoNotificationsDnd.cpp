/*
 * Copyright 2020 Kai Uwe Broulik <kde@broulik.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <obs-module.h>
#include <obs-frontend-api/obs-frontend-api.h>

#include <dbus/dbus.h>

#include <iostream>

#include "utils.h"

OBS_DECLARE_MODULE()

const char *obs_module_author()
{
    return "Kai Uwe Broulik";
}

const char *obs_module_name()
{
    return "Do not disturb mode while recording/streaming";
}

const char *obs_module_description()
{
    return "Enables do not disturb mode on supported desktops while streaming/recording is in progress.";
}

static DBusConnection *s_connection = nullptr;
// The current inhibition cookie as returned by the server
static uint s_cookie = 0;

// Inhibit notification with given reason
bool inhibit(const char *reason);
// Uninhibit current inhibition
bool uninhibit();

static const char *k_freedesktopNotificationService = "org.freedesktop.Notifications";
static const char *k_freedesktopNotificationPath = "/org/freedesktop/Notifications";
static const char *k_freedesktopNotificationInterface = "org.freedesktop.Notifications";

static const char *k_obsDesktopEntry = "obs"; // TODO can we query this?

static const char *k_debug = "FdoNotificationsDnd: ";

void obsstudio_fdo_notifications_dnd_frontend_event_callback(enum obs_frontend_event event, void *private_data)
{
    (void)private_data; // unused

    switch (event) {
    case OBS_FRONTEND_EVENT_STREAMING_STARTED:
        inhibit("Streaming in progress"); // TODO i18n
        break;
    case OBS_FRONTEND_EVENT_RECORDING_STARTED:
        inhibit("Recording in progress"); // TODO i18n
        break;

    case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
    case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
        uninhibit();

    default:
        break;
    }
}

bool obs_module_load()
{
    DBusError err;
    dbus_error_init(&err);

    s_connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        dbus_error_free(&err);
        return false;
    }

    if (!s_connection) {
        return false;
    }

    obs_frontend_add_event_callback(obsstudio_fdo_notifications_dnd_frontend_event_callback, nullptr);
    return true;
}

void obs_module_unload()
{
    if (!s_connection) {
        return;
    }

    dbus_connection_close(s_connection);
    s_connection = nullptr;
}

bool inhibit(const char *reason)
{
    if (s_cookie) {
        // So we can call inhibit() again with a different reason
        uninhibit();
    }

    std::cout << k_debug << "Requesting notification inhibition: " << reason << std::endl;

    DBusMessagePtr msg(dbus_message_new_method_call(
                           k_freedesktopNotificationService,
                           k_freedesktopNotificationPath,
                           k_freedesktopNotificationInterface,
                           "Inhibit"));
    if (!msg) {
        return false;
    }

    DBusMessageIter args;
    dbus_message_iter_init_append(msg.get(), &args);

    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &k_obsDesktopEntry)) {
        return false;
    }
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &reason)) {
        return false;
    }

    // Add empty "reserved" hashmap
    DBusMessageIter reservedArgs;
    if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &reservedArgs)) {
        return false;
    }
    // TODO abandon on fail?
    if (!dbus_message_iter_close_container(&args, &reservedArgs)) {
        return false;
    }

    DBusPendingCallPtr pendingCall;
    DBusPendingCall *pending;
    if (!dbus_connection_send_with_reply(s_connection, msg.get(), &pending, -1 /* default timeout*/)) {
        return false;
    }

    // Can I not pass the unqiue_ptr into the function above? With some horrific std::bind usage, maybe?
    pendingCall.reset(pending);

    if (!pendingCall) {
        return false;
    }

    dbus_connection_flush(s_connection);

    // TODO async :)
    dbus_pending_call_block(pendingCall.get());

    DBusMessagePtr reply(dbus_pending_call_steal_reply(pendingCall.get()));
    if (!reply) {
        return false;
    }

    if (!dbus_message_iter_init(reply.get(), &args)
            || dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_UINT32) {
        // Message has none or the wrong arguments (e.g. error)
        std::cerr << k_debug << "Failed to request notification inhibition: "
                  << dbus_message_get_error_name(reply.get())
                  << std::endl;
        return false;
    }

    dbus_message_iter_get_basic(&args, &s_cookie);
    return true;
}

bool uninhibit()
{
    if (!s_cookie) {
        return false;
    }

    std::cout << k_debug << "Removing notification inhibition with cookie " << s_cookie << std::endl;

    DBusMessagePtr msg(dbus_message_new_method_call(
                           k_freedesktopNotificationService,
                           k_freedesktopNotificationPath,
                           k_freedesktopNotificationInterface,
                           "UnInhibit"));
    if (!msg) {
        return false;
    }

    DBusMessageIter args;
    dbus_message_iter_init_append(msg.get(), &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &s_cookie)) {
        return false;
    }

    DBusPendingCallPtr pendingCall;
    DBusPendingCall *pending;
    if (!dbus_connection_send_with_reply(s_connection, msg.get(), &pending, -1 /* default timeout*/)) {
        return false;
    }

    dbus_connection_flush(s_connection);

    // TODO async :)
    dbus_pending_call_block(pending);

    DBusMessagePtr reply(dbus_pending_call_steal_reply(pending));
    if (!reply) {
        return false;
    }

    if (dbus_message_get_type(reply.get()) == DBUS_MESSAGE_TYPE_ERROR) {
        std::cerr << k_debug << "Failed to remove notification inhibition: "
                  << dbus_message_get_error_name(reply.get())
                  << std::endl;
        return false;
    }

    s_cookie = 0;
}
