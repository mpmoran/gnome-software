/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 * vi:set noexpandtab tabstop=8 shiftwidth=8:
 *
 * Copyright (C) 2018 Kalev Lember <klember@redhat.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include "config.h"

#include "gs-origin-popover-row.h"

#include <glib/gi18n.h>

typedef struct
{
	GsApp		*app;
	GtkWidget	*name_label;
	GtkWidget	*url_box;
	GtkWidget	*url_title;
	GtkWidget	*url_label;
	GtkWidget	*installation_box;
	GtkWidget	*installation_title;
	GtkWidget	*installation_label;
	GtkWidget	*branch_box;
	GtkWidget	*branch_title;
	GtkWidget	*branch_label;
	GtkWidget	*version_box;
	GtkWidget	*version_title;
	GtkWidget	*version_label;
	GtkWidget	*selected_image;
} GsOriginPopoverRowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GsOriginPopoverRow, gs_origin_popover_row, GTK_TYPE_LIST_BOX_ROW)

static void
refresh_ui (GsOriginPopoverRow *row)
{
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);
	g_autofree gchar *origin_ui = NULL;
	g_autofree gchar *url = NULL;

	g_assert (GS_IS_ORIGIN_POPOVER_ROW (row));
	g_assert (GS_IS_APP (priv->app));

	origin_ui = gs_app_get_origin_ui (priv->app);
	if (origin_ui != NULL) {
		gtk_label_set_text (GTK_LABEL (priv->name_label), origin_ui);
	}

	if (gs_app_get_state (priv->app) == GS_APP_STATE_AVAILABLE_LOCAL) {
		GFile *local_file = gs_app_get_local_file (priv->app);
		url = g_file_get_basename (local_file);
		/* TRANSLATORS: This is followed by a file name, e.g. "Name: gedit.rpm" */
		gtk_label_set_text (GTK_LABEL (priv->url_title), _("Name"));
	} else {
		url = g_strdup (gs_app_get_origin_hostname (priv->app));
	}

	if (url != NULL) {
		gtk_label_set_text (GTK_LABEL (priv->url_label), url);
		gtk_widget_show (priv->url_box);
	} else {
		gtk_widget_hide (priv->url_box);
	}

	if (gs_app_get_bundle_kind (priv->app) == AS_BUNDLE_KIND_FLATPAK &&
	    gs_app_get_scope (priv->app) != AS_COMPONENT_SCOPE_UNKNOWN) {
		AsComponentScope scope = gs_app_get_scope (priv->app);
		if (scope == AS_COMPONENT_SCOPE_SYSTEM) {
			/* TRANSLATORS: the installation location for flatpaks */
			gtk_label_set_text (GTK_LABEL (priv->installation_label), _("system"));
		} else if (scope == AS_COMPONENT_SCOPE_USER) {
			/* TRANSLATORS: the installation location for flatpaks */
			gtk_label_set_text (GTK_LABEL (priv->installation_label), _("user"));
		}
		gtk_widget_show (priv->installation_box);
	} else {
		gtk_widget_hide (priv->installation_box);
	}

	if (gs_app_get_branch (priv->app) != NULL) {
		gtk_label_set_text (GTK_LABEL (priv->branch_label), gs_app_get_branch (priv->app));
		gtk_widget_show (priv->branch_box);
	} else {
		gtk_widget_hide (priv->branch_box);
	}

	if (gs_app_get_bundle_kind (priv->app) == AS_BUNDLE_KIND_SNAP) {
		/* TRANSLATORS: the title for Snap channels */
		gtk_label_set_text (GTK_LABEL (priv->branch_title), _("Channel"));
		gtk_label_set_text (GTK_LABEL (priv->version_label), gs_app_get_version (priv->app));
		gtk_widget_show (priv->version_box);
	} else {
		/* TRANSLATORS: the title for Flatpak branches */
		gtk_label_set_text (GTK_LABEL (priv->branch_title), _("Branch"));
		gtk_widget_hide (priv->version_box);
	}
}

static void
gs_origin_popover_row_set_app (GsOriginPopoverRow *row, GsApp *app)
{
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);

	g_assert (priv->app == NULL);

	priv->app = g_object_ref (app);
	refresh_ui (row);
}

GsApp *
gs_origin_popover_row_get_app (GsOriginPopoverRow *row)
{
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);
	return priv->app;
}

void
gs_origin_popover_row_set_selected (GsOriginPopoverRow *row, gboolean selected)
{
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);

	gtk_widget_set_visible (priv->selected_image, selected);
}

void
gs_origin_popover_row_set_size_group (GsOriginPopoverRow *row, GtkSizeGroup *size_group)
{
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);

	gtk_size_group_add_widget (size_group, priv->url_title);
	gtk_size_group_add_widget (size_group, priv->installation_title);
	gtk_size_group_add_widget (size_group, priv->branch_title);
	gtk_size_group_add_widget (size_group, priv->version_title);
}

static void
gs_origin_popover_row_dispose (GObject *object)
{
	GsOriginPopoverRow *row = GS_ORIGIN_POPOVER_ROW (object);
	GsOriginPopoverRowPrivate *priv = gs_origin_popover_row_get_instance_private (row);

	g_clear_object (&priv->app);

	G_OBJECT_CLASS (gs_origin_popover_row_parent_class)->dispose (object);
}

static void
gs_origin_popover_row_init (GsOriginPopoverRow *row)
{
	gtk_widget_init_template (GTK_WIDGET (row));
}

static void
gs_origin_popover_row_class_init (GsOriginPopoverRowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->dispose = gs_origin_popover_row_dispose;

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Software/gs-origin-popover-row.ui");

	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, name_label);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, url_box);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, url_title);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, url_label);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, installation_box);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, installation_title);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, installation_label);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, branch_box);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, branch_title);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, branch_label);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, version_box);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, version_title);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, version_label);
	gtk_widget_class_bind_template_child_private (widget_class, GsOriginPopoverRow, selected_image);
}

GtkWidget *
gs_origin_popover_row_new (GsApp *app)
{
	GsOriginPopoverRow *row = g_object_new (GS_TYPE_ORIGIN_POPOVER_ROW, NULL);
	gs_origin_popover_row_set_app (row, app);
	return GTK_WIDGET (row);
}
