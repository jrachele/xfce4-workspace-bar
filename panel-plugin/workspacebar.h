/*  $Id$
 *
 *  Copyright (C) 2019 Julian Rachele
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __WORKSPACEBAR_H__
#define __WORKSPACEBAR_H__

G_BEGIN_DECLS

/* plugin structure */
typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget       *ebox;
    GtkWidget       *button;
    GtkWidget       *hvbox;
    GArray          *buttons;

    /* screen */
    WnckScreen      *screen;

}
WorkspaceBarPlugin;

// #define XFCE_TYPE_WORKSPACEBAR_PLUGIN            (workspacebar_get_type())
// #define XFCE_WORKSPACEBAR_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFCE_TYPE_WORKSPACEBAR_PLUGIN, WorkspaceBarPlugin))
// 
// GType workspacebar_get_type      (void) G_GNUC_CONST;
// void  workspacebar_register_type (XfcePanelTypeModule *type_module);

G_END_DECLS

#endif /* !__WORKSPACEBAR_H__ */
