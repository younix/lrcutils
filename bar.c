#include <gtk/gtk.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>

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

	value[10] = allocation.x;
	value[11] = allocation.x + allocation.width;

	XChangeProperty(display, handle, strut, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *) value, 4);
	XChangeProperty(display, handle, strut_partial, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *) value, 12);
}

int
main( int argc, char *argv[])
{
	GError *error = NULL;
	GtkWidget *widget;
	GtkWindow *window;
	GdkRectangle monitor;
	const gint height = 25;

	if (!gtk_init_with_args(&argc, &argv, "", entries, NULL, &error)) {
		if (error)
			g_printerr("Error: %s\n", error->message);
		return 1;
	}

	widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	window = GTK_WINDOW(widget);

	gdk_screen_get_monitor_geometry(gtk_widget_get_screen(widget),
					monitor_id, &monitor);

	gtk_window_set_default_size(window, monitor.width, height);

	gtk_window_set_type_hint(window, GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
	gtk_window_set_decorated(window, FALSE);
	gtk_window_set_skip_taskbar_hint(window, TRUE);
	gtk_window_set_skip_pager_hint(window, TRUE);
	gtk_window_stick(window);
	gtk_window_set_keep_above(window, TRUE);
	gtk_window_set_keep_below(window, FALSE);
	gtk_window_set_deletable(window, FALSE);

	gtk_window_move(window, monitor.x, monitor.height - height);

	gtk_widget_show(widget);

	if (!overlap)
		claim_area(widget, &monitor);

	gtk_main();

	return 0;
}
