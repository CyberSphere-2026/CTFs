#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define FLAG_LEN 51


static const unsigned char enc1[] = {
    0x63, 0x59, 0x42, 0x45, 0x52, 0x53, 0x50, 0x48, 0x45, 0x52, 0x45, 0x5B, 0x79, 0x10, 0x55, 0x7F, 0x53
}; 

static const unsigned char enc2[] = {
    0x4E, 0x16, 0x53, 0x4A, 0x42, 0x79, 0x4A, 0x43, 0x12, 0x50, 0x15, 0x79, 0x51, 0x4E, 0x17, 0x4A, 0x15
}; 

static const unsigned char enc3[] = {
    0x2F, 0x09, 0x40, 0x05, 0x2F, 0x03, 0x04, 0x41, 0x1C, 0x1C, 0x2F, 0x13, 0x44, 0x1E, 0x51, 0x51, 0x0D
}; 

static int check_flag(const char *input) {
    size_t len = strlen(input);
    if (len != FLAG_LEN) return 0;

    for (int i = 0; i < 17; i++)
        if (((unsigned char)input[i] ^ 0x20) != enc1[i]) return 0;

    for (int i = 0; i < 17; i++)
        if (((unsigned char)input[17+i] ^ 0x26) != enc2[i]) return 0;

    for (int i = 0; i < 17; i++)
        if (((unsigned char)input[34+i] ^ 0x70) != enc3[i]) return 0;

    return 1;
}

static GtkWidget *entry_flag;
static GtkWidget *lbl_result;
static GtkWidget *overlay_box;   
static GdkPixbuf *bg_pixbuf = NULL;
static int        bg_w = 0, bg_h = 0;

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);

    if (bg_pixbuf) {
        GdkPixbuf *scaled = gdk_pixbuf_scale_simple(
            bg_pixbuf, alloc.width, alloc.height, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scaled, 0, 0);
        cairo_paint(cr);
        g_object_unref(scaled);
    }

    cairo_pattern_t *vignette = cairo_pattern_create_radial(
        alloc.width * 0.5, alloc.height * 0.5, alloc.height * 0.15,
        alloc.width * 0.5, alloc.height * 0.5, alloc.height * 0.85);
    cairo_pattern_add_color_stop_rgba(vignette, 0.0, 0,0,0, 0.0);
    cairo_pattern_add_color_stop_rgba(vignette, 1.0, 0,0,0, 0.72);
    cairo_set_source(cr, vignette);
    cairo_paint(cr);
    cairo_pattern_destroy(vignette);

    return FALSE;
}

static void on_submit(GtkWidget *widget, gpointer data) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry_flag));
    int ok = check_flag(text);

    if (ok) {
        gtk_label_set_text(GTK_LABEL(lbl_result), "CORRECT!! Well done!");
        GtkStyleContext *ctx = gtk_widget_get_style_context(overlay_box);
        gtk_style_context_remove_class(ctx, "wrong");
        gtk_style_context_add_class(ctx, "correct");
    } else {
        gtk_label_set_text(GTK_LABEL(lbl_result), "INCORRECT!! You're stuck here!");
        GtkStyleContext *ctx = gtk_widget_get_style_context(overlay_box);
        gtk_style_context_remove_class(ctx, "correct");
        gtk_style_context_add_class(ctx, "wrong");
    }
    gtk_widget_show(overlay_box);
}

static void on_entry_activate(GtkWidget *widget, gpointer data) {
    on_submit(NULL, NULL);
}

static const char *CSS =
    "window { background: transparent; }"

    ".card {"
    "  background: rgba(5,8,14,0.72);"
    "  border: 1px solid rgba(100,130,160,0.25);"
    "  border-radius: 4px;"
    "  padding: 36px 44px 32px 44px;"
    "}"

    ".title {"
    "  font-family: 'IM Fell English', 'Palatino Linotype', serif;"
    "  font-size: 46px;"
    "  font-weight: bold;"
    "  color: #c9d8e8;"
    "  letter-spacing: 4px;"
    "  text-shadow: 0 0 24px rgba(80,160,220,0.55);"
    "}"

    ".rule {"
    "  background: linear-gradient(90deg,"
    "    transparent, rgba(100,170,220,0.6), transparent);"
    "  min-height: 1px;"
    "  margin: 10px 0 26px 0;"
    "}"

    ".subtitle {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  color: rgba(160,190,210,0.65);"
    "  letter-spacing: 2px;"
    "  margin-bottom: 14px;"
    "}"

    ".flag-entry {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 15px;"
    "  color: #ddeeff;"
    "  background: rgba(0,10,20,0.82);"
    "  border: 1px solid rgba(80,130,180,0.45);"
    "  border-radius: 3px;"
    "  padding: 10px 14px;"
    "  caret-color: #6ab4e8;"
    "}"
    ".flag-entry:focus {"
    "  border-color: rgba(80,180,240,0.8);"
    "  box-shadow: 0 0 0 2px rgba(80,160,220,0.18);"
    "}"

    ".submit-btn {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  letter-spacing: 3px;"
    "  color: #b0d4ee;"
    "  background: rgba(20,50,80,0.8);"
    "  border: 1px solid rgba(80,150,200,0.5);"
    "  border-radius: 3px;"
    "  padding: 10px 28px;"
    "  transition: background 120ms;"
    "}"
    ".submit-btn:hover {"
    "  background: rgba(30,80,120,0.9);"
    "  color: #e0f0ff;"
    "}"
    ".submit-btn:active {"
    "  background: rgba(10,30,55,0.95);"
    "}"

    ".result-box {"
    "  border-radius: 3px;"
    "  padding: 10px 16px;"
    "  margin-top: 18px;"
    "}"
    ".result-box.correct {"
    "  background: rgba(0,60,30,0.80);"
    "  border: 1px solid rgba(0,200,80,0.4);"
    "}"
    ".result-box.wrong {"
    "  background: rgba(60,0,0,0.80);"
    "  border: 1px solid rgba(200,30,30,0.4);"
    "}"
    ".result-lbl {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  color: #ddeeff;"
    "  letter-spacing: 1px;"
    "}"

    ".author {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  color: rgba(120,150,170,0.55);"
    "  letter-spacing: 1px;"
    "}";

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    const char *bg_path = "background.jpg";
    GError *err = NULL;
    bg_pixbuf = gdk_pixbuf_new_from_file(bg_path, &err);
    if (!bg_pixbuf) {
        g_print("Warning: could not load background '%s': %s\n",
                bg_path, err ? err->message : "unknown error");
        if (err) g_error_free(err);
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dead in the Water");
    gtk_window_set_default_size(GTK_WINDOW(window), 860, 560);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing = gtk_drawing_area_new();
    g_signal_connect(drawing, "draw", G_CALLBACK(on_draw), NULL);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(overlay), drawing);

    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(outer, 0);
    gtk_widget_set_margin_end(outer, 0);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");
    gtk_widget_set_size_request(card, 540, -1);

    GtkWidget *title = gtk_label_new("Dead in the Water");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
    gtk_label_set_xalign(GTK_LABEL(title), 0.5);
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    GtkWidget *rule = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(rule), "rule");
    gtk_box_pack_start(GTK_BOX(card), rule, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("ENTER THE FLAG");
    gtk_style_context_add_class(gtk_widget_get_style_context(subtitle), "subtitle");
    gtk_label_set_xalign(GTK_LABEL(subtitle), 0.5);
    gtk_box_pack_start(GTK_BOX(card), subtitle, FALSE, FALSE, 0);

    entry_flag = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_flag), "Cybersphere{...}");
    gtk_entry_set_max_length(GTK_ENTRY(entry_flag), 128);
    gtk_entry_set_visibility(GTK_ENTRY(entry_flag), TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(entry_flag), "flag-entry");
    g_signal_connect(entry_flag, "activate", G_CALLBACK(on_entry_activate), NULL);
    gtk_box_pack_start(GTK_BOX(card), entry_flag, FALSE, FALSE, 10);

    GtkWidget *btn = gtk_button_new_with_label("SUBMIT");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "submit-btn");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_submit), NULL);
    gtk_box_pack_start(GTK_BOX(card), btn, FALSE, FALSE, 4);

    overlay_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(overlay_box), "result-box");
    lbl_result = gtk_label_new("");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_result), "result-lbl");
    gtk_label_set_xalign(GTK_LABEL(lbl_result), 0.5);
    gtk_box_pack_start(GTK_BOX(overlay_box), lbl_result, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(card), overlay_box, FALSE, FALSE, 0);
    gtk_widget_hide(overlay_box);

    gtk_box_pack_start(GTK_BOX(outer), card, FALSE, FALSE, 0);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), outer);

    GtkWidget *author = gtk_label_new("Author : OTC");
    gtk_style_context_add_class(gtk_widget_get_style_context(author), "author");
    gtk_widget_set_valign(author, GTK_ALIGN_END);
    gtk_widget_set_halign(author, GTK_ALIGN_START);
    gtk_widget_set_margin_start(author, 14);
    gtk_widget_set_margin_bottom(author, 10);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), author);

    gtk_container_add(GTK_CONTAINER(window), overlay);
    gtk_widget_show_all(window);
    gtk_widget_hide(overlay_box);   

    gtk_main();
    return 0;
}