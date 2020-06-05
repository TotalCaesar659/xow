/*
 * Copyright (C) 2020 Medusalix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "notify.h"
#include "log.h"

#include <systemd/sd-bus.h>

#define NOTIFY_SERVICE "org.freedesktop.Notifications"
#define NOTIFY_PATH "/org/freedesktop/Notifications"
#define NOTIFY_INTERFACE "org.freedesktop.Notifications"

#define NOTIFY_METHOD "Notify"
#define NOTIFY_SIGNATURE "susssasa{sv}i"

#define NOTIFY_APP_NAME "xow"
#define NOTIFY_APP_ICON "input-gaming"
#define NOTIFY_CATEGORY "device"

namespace Notify
{
    sd_bus_message* createMessage(
        sd_bus *bus,
        Notification notification
    ) {
        sd_bus_message *message = nullptr;
        int error = sd_bus_message_new_method_call(
            bus,
            &message,
            NOTIFY_SERVICE,
            NOTIFY_PATH,
            NOTIFY_INTERFACE,
            NOTIFY_METHOD
        );

        if (error < 0)
        {
            Log::error("Error creating notify call: %s", strerror(-error));

            return nullptr;
        }

        error = sd_bus_message_append(
            message,
            NOTIFY_SIGNATURE,
            NOTIFY_APP_NAME,
            0,
            NOTIFY_APP_ICON,
            notification.summary.c_str(),
            notification.body.c_str(),
            0,
            3,
            "category",
            "s",
            NOTIFY_CATEGORY,
            "sound-name",
            "s",
            notification.sound.c_str(),
            "urgency",
            "y",
            notification.urgency,
            -1,
            nullptr
        );

        if (error < 0)
        {
            Log::error("Error filling notify message: %s", strerror(-error));

            return nullptr;
        }

        return message;
    }

    bool showNotification(Notification notification)
    {
        sd_bus *bus = nullptr;
        int error = sd_bus_default_user(&bus);

        if (error < 0)
        {
            Log::error("Error connecting to session bus: %s", strerror(-error));

            return false;
        }

        sd_bus_message *message = createMessage(bus, notification);

        if (!message)
        {
            return false;
        }

        error = sd_bus_call(
            bus,
            message,
            0,
            nullptr,
            nullptr
        );

        sd_bus_message_unref(message);

        if (error < 0)
        {
            Log::error("Error calling notify: %s", strerror(-error));

            return false;
        }

        return true;
    }
}
