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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libwnck/libwnck.h>

#include "workspacebar.h"


/* prototypes */
static void
workspacebar_construct (XfcePanelPlugin *plugin);

static void      
workspacebar_screen_changed            (GtkWidget          *widget,
                                        GdkScreen          *previous_screen,
                                        WorkspaceBarPlugin *plugin);
static void      
workspacebar_active_window_changed   (WnckScreen         *screen,
                                      WnckWindow         *previous_window,
                                      WorkspaceBarPlugin   *plugin);

static void      
workspacebar_active_workspace_changed  (WnckScreen         *screen,
                                        WnckWorkspace      *previous_workspace,
                                        WorkspaceBarPlugin   *plugin);

/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (workspacebar_construct);

static WorkspaceBarPlugin *
workspacebar_new (XfcePanelPlugin *plugin)
{
  WorkspaceBarPlugin   *workspacebar;
  GtkOrientation  orientation;
  // GtkWidget      *label;

  /* allocate memory for the plugin structure */
  workspacebar = g_slice_new0 (WorkspaceBarPlugin);

  /* pointer to plugin */
  workspacebar->plugin = plugin;

  /* get the current orientation */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* create some panel widgets */
  workspacebar->ebox = gtk_event_box_new ();
  gtk_widget_show (workspacebar->ebox);

  workspacebar->hvbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_widget_show (workspacebar->hvbox);
  gtk_container_add (GTK_CONTAINER (workspacebar->ebox), workspacebar->hvbox);

  return workspacebar;
}



static void
workspacebar_free (XfcePanelPlugin *plugin,
                   WorkspaceBarPlugin    *workspacebar)
{
  /* destroy the panel widgets */
  gtk_widget_destroy (workspacebar->ebox);
  gtk_widget_destroy (workspacebar->hvbox);

  g_signal_handlers_disconnect_by_func (G_OBJECT (plugin),
        workspacebar_screen_changed, workspacebar);

  if (G_UNLIKELY (workspacebar->screen != NULL))
    {
      /* disconnect from all windows on the old screen */
      // workspacebar_windows_disconnect (plugin);

      /* disconnect from the previous screen */
      g_signal_handlers_disconnect_by_func (G_OBJECT (workspacebar->screen),
          workspacebar_active_window_changed, plugin);
      g_signal_handlers_disconnect_by_func (G_OBJECT (workspacebar->screen),
          workspacebar_active_workspace_changed, plugin);

      workspacebar->screen = NULL;
    }

  /* free the plugin structure */
  g_slice_free (WorkspaceBarPlugin, workspacebar);
}



static void
workspacebar_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            WorkspaceBarPlugin    *workspacebar)
{
  /* change the orienation of the box */
  gtk_orientable_set_orientation(GTK_ORIENTABLE(workspacebar->hvbox), orientation);
}


static gboolean
workspacebar_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     WorkspaceBarPlugin    *workspacebar)
{
  GtkOrientation orientation;

  /* get the orientation of the plugin */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* set the widget size */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
  else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

  /* we handled the orientation */
  return TRUE;
}

static void
workspacebar_screen_changed (GtkWidget *widget,
                             GdkScreen *previous_screen,
                             WorkspaceBarPlugin *plugin)

{
  GdkScreen        *screen;
  WnckScreen       *wnck_screen;
  WnckScreen       *wnck_previous_screen;

  // if (plugin == NULL) return;

  g_message(_("Screen changed."));

  /* get the wnck screen */
  screen = gtk_widget_get_screen (widget);
  _panel_return_if_fail (GDK_IS_SCREEN (screen));
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  wnck_screen = wnck_screen_get (gdk_screen_get_number (screen));
  wnck_previous_screen = wnck_screen_get(gdk_screen_get_number(previous_screen));
G_GNUC_END_IGNORE_DEPRECATIONS
  _panel_return_if_fail (WNCK_IS_SCREEN (wnck_screen));

  /* leave when we same wnck screen was picked */
  if (plugin->screen == wnck_screen)
    return;

  if (G_UNLIKELY (plugin->screen != NULL))
    {
      /* disconnect from all windows on the old screen */
      // workspacebar_windows_disconnect (plugin);

      /* disconnect from the previous screen */
      g_signal_handlers_disconnect_by_func (G_OBJECT (plugin->screen),
          workspacebar_active_window_changed, plugin);
      g_signal_handlers_disconnect_by_func (G_OBJECT (plugin->screen),
          workspacebar_active_workspace_changed, plugin);

    }

  /* set the new screen */
  plugin->screen = wnck_screen;

  /* connect signal to monitor this screen */
  g_signal_connect (G_OBJECT (plugin->screen), "active-window-changed",
      G_CALLBACK (workspacebar_active_window_changed), plugin);
  g_signal_connect (G_OBJECT (plugin->screen), "active-workspace-changed",
      G_CALLBACK (workspacebar_active_workspace_changed), plugin);

}

static void      
workspacebar_active_window_changed   (WnckScreen         *screen,
                                      WnckWindow         *previous_window,
                                      WorkspaceBarPlugin   *plugin) 
{
  PangoFontDescription *italic = pango_font_description_from_string("bold italic");

  if (screen == NULL) return;

  // Clear the ebox of all current button objects
  GList *buttons = gtk_container_get_children(plugin->hvbox);
  GList *listitem;
  for (listitem = g_list_first(buttons); listitem != NULL; listitem = listitem->next) {
    GtkWidget *b = listitem->data;
    gtk_widget_destroy(b);
  }

  // GtkOrientation orientation = xfce_panel_plugin_get_orientation (widget);
  WnckWorkspace *active_workspace = wnck_screen_get_active_workspace(screen);
  GList *windows = wnck_screen_get_windows_stacked(screen);
  GList *windowitem;
  GList *workspaces = wnck_screen_get_workspaces(screen);
  GList *workspaceitem;

  /* Iterate through workspaces */
  for (workspaceitem = g_list_first(workspaces); workspaceitem != NULL; workspaceitem = workspaceitem->next) {
    /* We must determine the icon for the workspace */
    int valid = 0;
    WnckWorkspace *workspace = workspaceitem->data;
    GtkWidget *icon = gtk_image_new_from_icon_name("user-desktop", GTK_ICON_SIZE_BUTTON);
    for (windowitem = g_list_first(windows); windowitem != NULL; windowitem = windowitem->next) {
        WnckWindow *window = windowitem->data;
        if (wnck_window_get_workspace(window) == workspace) {
           gtk_image_set_from_pixbuf(icon, wnck_window_get_mini_icon(window));
           valid = 1;
        }
    }
    if (valid || workspace == active_workspace) {
      /* If the workspace was either active or had an active window on it*/
      GtkWidget *button = xfce_arrow_button_new (GTK_ARROW_NONE);
      gtk_container_add (GTK_CONTAINER (plugin->hvbox), button);
      gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
      gtk_widget_show (button);

      GtkWidget *hvbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
      gtk_widget_show (hvbox);
      gtk_container_add (GTK_CONTAINER (button), hvbox);

      gtk_container_add (GTK_CONTAINER (hvbox), icon);
      gtk_widget_show (icon);

      int workspace_num = wnck_workspace_get_number(workspace) + 1;
      gchar *display = g_strdup_printf("%d", workspace_num);
      GtkWidget *label = gtk_label_new(_(display));
      if (workspace == active_workspace) gtk_widget_modify_font(label, italic);
      g_free(display);
      gtk_widget_show (label);
      gtk_container_add (GTK_CONTAINER (hvbox), label);
    }
  }
}

static void
workspacebar_active_workspace_changed      (WnckScreen       *screen,
                                            WnckWorkspace    *previous_workspace,
                                            WorkspaceBarPlugin *plugin)
{
  /* refresh the top bar */
  workspacebar_active_window_changed(screen, NULL, plugin);
}


static void
workspacebar_construct (XfcePanelPlugin *plugin)
{
  WorkspaceBarPlugin *workspacebar;

  /* create the plugin */
  workspacebar = workspacebar_new (plugin);

  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), workspacebar->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, workspacebar->ebox);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (workspacebar_free), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (workspacebar_size_changed), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (workspacebar_orientation_changed), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "screen-changed",
                    G_CALLBACK (workspacebar_screen_changed), workspacebar);

  workspacebar_screen_changed(GTK_WIDGET (plugin), NULL, workspacebar);
}
