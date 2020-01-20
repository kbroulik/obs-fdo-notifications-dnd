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

#pragma once

#include <memory>

// Some cleanup helpers so we don't have to cope with this manual "release" nonsense :)
struct DBusMessageDeleter
{
    void operator()(DBusMessage *msg) const {
        if (msg) {
            dbus_message_unref(msg);
        }
    }
};
using DBusMessagePtr = std::unique_ptr<DBusMessage, DBusMessageDeleter>;

struct DBusPendingCallDeleter
{
    void operator()(DBusPendingCall *call) const {
        if (call) {
            dbus_pending_call_unref(call);
        }
    }
};
using DBusPendingCallPtr = std::unique_ptr<DBusPendingCall, DBusPendingCallDeleter>;
