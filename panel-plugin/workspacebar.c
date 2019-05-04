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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "workspacebar.h"
#include "workspacebar-dialogs.h"

/* default settings */
#define DEFAULT_SETTING1 NULL
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE



/* prototypes */
static void
workspacebar_construct (XfcePanelPlugin *plugin);


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (workspacebar_construct);



void
workspacebar_save (XfcePanelPlugin *plugin,
             WorkspaceBarPlugin    *workspacebar)
{
  XfceRc *rc;
  gchar  *file;

  /* get the config file location */
  file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }

  /* open the config file, read/write */
  rc = xfce_rc_simple_open (file, FALSE);
  g_free (file);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      DBG(".");
      if (workspacebar->setting1)
        xfce_rc_write_entry    (rc, "setting1", workspacebar->setting1);

      xfce_rc_write_int_entry  (rc, "setting2", workspacebar->setting2);
      xfce_rc_write_bool_entry (rc, "setting3", workspacebar->setting3);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}



static void
workspacebar_read (WorkspaceBarPlugin *workspacebar)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (workspacebar->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      rc = xfce_rc_simple_open (file, TRUE);

      /* cleanup */
      g_free (file);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          value = xfce_rc_read_entry (rc, "setting1", DEFAULT_SETTING1);
          workspacebar->setting1 = g_strdup (value);

          workspacebar->setting2 = xfce_rc_read_int_entry (rc, "setting2", DEFAULT_SETTING2);
          workspacebar->setting3 = xfce_rc_read_bool_entry (rc, "setting3", DEFAULT_SETTING3);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  workspacebar->setting1 = g_strdup (DEFAULT_SETTING1);
  workspacebar->setting2 = DEFAULT_SETTING2;
  workspacebar->setting3 = DEFAULT_SETTING3;
}



static WorkspaceBarPlugin *
workspacebar_new (XfcePanelPlugin *plugin)
{
  WorkspaceBarPlugin   *workspacebar;
  GtkOrientation  orientation;
  GtkWidget      *label;

  /* allocate memory for the plugin structure */
  workspacebar = g_slice_new0 (WorkspaceBarPlugin);

  /* pointer to plugin */
  workspacebar->plugin = plugin;

  /* read the user settings */
  workspacebar_read (workspacebar);

  /* get the current orientation */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* create some panel widgets */
  workspacebar->ebox = gtk_event_box_new ();
  gtk_widget_show (workspacebar->ebox);

  workspacebar->hvbox = gtk_box_new (orientation, 2);
  gtk_widget_show (workspacebar->hvbox);
  gtk_container_add (GTK_CONTAINER (workspacebar->ebox), workspacebar->hvbox);

  /* some workspacebar widgets */
  label = gtk_label_new (_("WorkspaceBar"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (workspacebar->hvbox), label, FALSE, FALSE, 0);

  label = gtk_label_new (_("Plugin"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (workspacebar->hvbox), label, FALSE, FALSE, 0);

  return workspacebar;
}



static void
workspacebar_free (XfcePanelPlugin *plugin,
             WorkspaceBarPlugin    *workspacebar)
{
  GtkWidget *dialog;

  /* check if the dialog is still open. if so, destroy it */
  dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
  if (G_UNLIKELY (dialog != NULL))
    gtk_widget_destroy (dialog);

  /* destroy the panel widgets */
  gtk_widget_destroy (workspacebar->hvbox);

  /* cleanup the settings */
  if (G_LIKELY (workspacebar->setting1 != NULL))
    g_free (workspacebar->setting1);

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
workspacebar_construct (XfcePanelPlugin *plugin)
{
  WorkspaceBarPlugin *workspacebar;

  /* setup transation domain */
  xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* create the plugin */
  workspacebar = workspacebar_new (plugin);

  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), workspacebar->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, workspacebar->ebox);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (workspacebar_free), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (workspacebar_save), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (workspacebar_size_changed), workspacebar);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (workspacebar_orientation_changed), workspacebar);

  /* show the configure menu item and connect signal */
  xfce_panel_plugin_menu_show_configure (plugin);
  g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (workspacebar_configure), workspacebar);

  /* show the about menu item and connect signal */
  xfce_panel_plugin_menu_show_about (plugin);
  g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (workspacebar_about), NULL);
}
