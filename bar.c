#include <gtk/gtk.h>
#include <glib.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>

#include <unistd.h>

static gint monitor_id = 0;
static gboolean overlap = FALSE;
static GOptionEntry entries[] = 
{
  { "monitor", 'n', 0, G_OPTION_ARG_INT, &monitor_id,
	  "Display on Monitor N (default 0)", "N" },
  { "overlap", 'o', 0, G_OPTION_ARG_NONE, &overlap,
	  "Allow maximized windows to overlap", NULL },
  { NULL }
};

static void
claim_area(GtkWidget *widget, GdkRectangle *monitor)
{
	GtkAllocation allocation;
	GdkWindow *gdk_window;
	Window handle;
	Display *display;
	Atom strut, strut_partial;
	long value[12] = { 0 };

	gtk_widget_get_allocation(widget, &allocation);
	gdk_window = gtk_widget_get_window(widget);
	handle = GDK_WINDOW_XID(gdk_window);
	display = GDK_WINDOW_XDISPLAY(gdk_window);
	strut = XInternAtom(display, "_NET_WM_STRUT", FALSE);
	strut_partial = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", FALSE);

	/* According to _NET_WM spec the size is relative to screen size,
	 * but at least awesome interprets it relative to output-size. See:
	 * http://standards.freedesktop.org/wm-spec/wm-spec-1.4.html#id2560850
	 * Thus, it should correctly be:
	 * value[3] = gdk_screen_height() - monitor->height + allocation.height;
	 */
	value[3] = allocation.height;

	value[10] = monitor->x;
	value[11] = monitor->x + allocation.width;

	XChangeProperty(display, handle, strut, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *) value, 4);
	XChangeProperty(display, handle, strut_partial, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *) value, 12);
}

static inline void
set_label(GtkWidget *label, gchar *str)
{
	gchar *fstr;
	
	fstr = g_strdup_printf("<span font_desc=\"20\">%s</span>", str);
	gtk_label_set_markup(GTK_LABEL(label), fstr);
	g_free(fstr);
}

static gboolean
read_cb(GIOChannel *ch, GIOCondition condition, gpointer data)
{
	GtkWidget *text = data;
	gchar buf[BUFSIZ], *nl;
	gsize len = 0;
	GError *err = NULL;
	GString *content = g_object_get_data(G_OBJECT(text), "string");

	switch (condition) {
	case G_IO_IN:
		switch (g_io_channel_read_chars(ch, buf, BUFSIZ, &len, &err)) {
		case G_IO_STATUS_EOF:
			gtk_main_quit();
			break;
		case G_IO_STATUS_AGAIN:
			return read_cb(ch, condition, data);
			break;
		case G_IO_STATUS_ERROR:
			if (err) {
				g_printerr("Error: %s", err->message);
				gtk_main_quit();
			}
			break;
		case G_IO_STATUS_NORMAL:
		default:
			if (len <= 0)
				break;
			content = g_string_append_len(content, buf, len);
			if ((nl = g_strrstr(content->str, "\n")) != NULL)
				*nl = '\0';
			set_label(text, content->str);
			if (nl != NULL) /* Reset string if found a newline */
				content = g_string_truncate(content, 0);
			break;
		}
		break;
	case G_IO_HUP:
		gtk_main_quit();
		break;
	default:
		break;
	}

	return TRUE;
}

int
main( int argc, char *argv[])
{
	GError *error = NULL;
	GtkWidget *widget;
	GtkWindow *window;
	GdkScreen *screen;
	GtkAllocation allocation;
	GtkWidget *text;
	GdkRectangle monitor;
	GIOChannel *input_channel;

	if (!gtk_init_with_args(&argc, &argv, "", entries, NULL, &error)) {
		if (error)
			g_printerr("Error: %s\n", error->message);
		return 1;
	}

	widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	window = GTK_WINDOW(widget);

	screen = gtk_widget_get_screen(widget);
	if (monitor_id >= gdk_screen_get_n_monitors(screen)) {
		g_printerr("Error: Invalid monitor id\n");
		return 1;
	}

	gdk_screen_get_monitor_geometry(screen, monitor_id, &monitor);

	gtk_window_set_default_size(window, monitor.width, -1);
	/* Dont know height yet, move to invisible area for now */
	gtk_window_move(window, monitor.x, monitor.height);

	gtk_window_set_type_hint(window, GDK_WINDOW_TYPE_HINT_DOCK);
	gtk_window_set_decorated(window, FALSE);
	gtk_window_set_skip_taskbar_hint(window, TRUE);
	gtk_window_set_skip_pager_hint(window, TRUE);
	gtk_window_stick(window);
	gtk_window_set_keep_above(window, TRUE);
	gtk_window_set_keep_below(window, FALSE);
	gtk_window_set_deletable(window, FALSE);

	text = gtk_label_new(NULL);
	gtk_container_add(GTK_CONTAINER(widget), text);
	gtk_widget_show(text);
	set_label(text, " ");
	g_object_set_data(G_OBJECT(text), "string", g_string_new(""));

	gtk_widget_show(widget);

	gtk_widget_get_allocation(widget, &allocation);
	gtk_window_move(window, monitor.x, monitor.height - allocation.height);

	if (!overlap)
		claim_area(widget, &monitor);

	input_channel = g_io_channel_unix_new(STDIN_FILENO);
	g_io_channel_set_flags(input_channel, G_IO_FLAG_NONBLOCK, &error);
	g_io_add_watch(input_channel, G_IO_IN | G_IO_HUP, read_cb, text);

	gtk_main();

	g_io_channel_unref(input_channel);
	g_string_free(g_object_get_data(G_OBJECT(text), "string"), TRUE);

	return 0;
}
