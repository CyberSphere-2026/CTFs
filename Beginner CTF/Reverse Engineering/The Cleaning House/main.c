#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>



static uint8_t nibble_swap(uint8_t b) {
    return ((b & 0x0F) << 4) | ((b & 0xF0) >> 4);
}

static uint8_t rotl8(uint8_t b, int n) {
    return (uint8_t)((b << n) | (b >> (8 - n)));
}

static uint8_t odd_even_transform(uint8_t b) {
    if (b & 1)
        return (uint8_t)((b + 0x13) & 0xFF);
    else
        return (uint8_t)((b - 0x47) & 0xFF);
}

static void xor_with_rand(uint8_t *buf, size_t len, uint16_t seed) {
    srand((unsigned int)seed);
    for (size_t i = 0; i < len; i++)
        buf[i] ^= (uint8_t)(rand() & 0xFF);
}

static void reverse_buf(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len / 2; i++) {
        uint8_t tmp  = buf[i];
        buf[i]       = buf[len - 1 - i];
        buf[len-1-i] = tmp;
    }
}

static const char KEY[] = "CINEAMA";
#define KEYLEN 7

static void xor_with_key(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++)
        buf[i] ^= (uint8_t)KEY[i % KEYLEN];
}

static char *encrypt(const char *input) {
    size_t len = strlen(input);
    if (len == 0) return g_strdup("");

    uint8_t *buf = (uint8_t *)malloc(len);
    memcpy(buf, input, len);

    for (size_t i = 0; i < len; i++)
        buf[i] = nibble_swap(buf[i]);

    for (size_t i = 0; i < len; i++)
        buf[i] = rotl8(buf[i], 3);

    for (size_t i = 0; i < len; i++)
        buf[i] = odd_even_transform(buf[i]);

    uint16_t seed = (uint16_t)(time(NULL) & 0xFFFF);
    xor_with_rand(buf, len, seed);

    reverse_buf(buf, len);

    xor_with_key(buf, len);

    char *hex = (char *)malloc(len * 2 + 1);
    for (size_t i = 0; i < len; i++)
        snprintf(hex + i * 2, 3, "%02X", buf[i]);
    hex[len * 2] = '\0';

    free(buf);
    return hex;   
}

static GtkWidget  *entry_input;
static GtkWidget  *lbl_output;
static GtkWidget  *lbl_seed;
static GdkPixbuf  *bg_pixbuf = NULL;


static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    GtkAllocation a;
    gtk_widget_get_allocation(widget, &a);

    if (bg_pixbuf) {
        GdkPixbuf *sc = gdk_pixbuf_scale_simple(
            bg_pixbuf, a.width, a.height, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, sc, 0, 0);
        cairo_paint(cr);
        g_object_unref(sc);
    } else {
        cairo_set_source_rgb(cr, 0.04, 0.04, 0.06);
        cairo_paint(cr);
    }

    cairo_pattern_t *vig = cairo_pattern_create_radial(
        a.width*.5, a.height*.5, a.height*.1,
        a.width*.5, a.height*.5, a.height*.9);
    cairo_pattern_add_color_stop_rgba(vig, 0.0, 0,0,0, 0.0);
    cairo_pattern_add_color_stop_rgba(vig, 1.0, 0,0,0, 0.78);
    cairo_set_source(cr, vig);
    cairo_paint(cr);
    cairo_pattern_destroy(vig);

    return FALSE;
}

static void on_encrypt(GtkWidget *w, gpointer data) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry_input));
    if (!text || strlen(text) == 0) {
        gtk_label_set_text(GTK_LABEL(lbl_output), "—");
        return;
    }

    uint16_t seed = (uint16_t)(time(NULL) & 0xFFFF);
    char *hex = encrypt(text);

    gtk_label_set_text(GTK_LABEL(lbl_output), hex);

    free(hex);
}

static void on_enter(GtkWidget *w, gpointer data) { on_encrypt(NULL, NULL); }



static const char *CSS =
    "window { background: transparent; }"

    ".card {"
    "  background: rgba(8,4,12,0.78);"
    "  border: 1px solid rgba(220,50,50,0.22);"
    "  border-top: 2px solid rgba(255,80,60,0.55);"
    "  border-radius: 2px;"
    "  padding: 38px 48px 32px 48px;"
    "}"

    ".title {"
    "  font-family: 'Bebas Neue', 'Impact', 'Arial Narrow', sans-serif;"
    "  font-size: 52px;"
    "  letter-spacing: 10px;"
    "  color: #ff4433;"
    "  text-shadow: 0 0 18px rgba(255,60,40,0.9),"
    "               0 0 40px rgba(255,30,10,0.5);"
    "}"

    ".neon-bar {"
    "  background: linear-gradient(90deg,"
    "    transparent 0%, rgba(255,60,40,0.8) 20%,"
    "    rgba(255,100,60,1.0) 50%,"
    "    rgba(255,60,40,0.8) 80%, transparent 100%);"
    "  min-height: 2px;"
    "  margin: 6px 0 28px 0;"
    "  border-radius: 1px;"
    "}"

    ".prompt {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  letter-spacing: 3px;"
    "  color: rgba(255,120,80,0.65);"
    "  margin-bottom: 8px;"
    "}"

    ".input-box {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 15px;"
    "  color: #ffd0c0;"
    "  background: rgba(0,0,0,0.75);"
    "  border: 1px solid rgba(200,60,40,0.4);"
    "  border-radius: 2px;"
    "  padding: 10px 14px;"
    "  caret-color: #ff4433;"
    "}"
    ".input-box:focus {"
    "  border-color: rgba(255,80,50,0.9);"
    "  box-shadow: 0 0 0 2px rgba(255,60,30,0.18);"
    "}"

    ".enc-btn {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 12px;"
    "  letter-spacing: 4px;"
    "  color: #ff6040;"
    "  background: rgba(30,5,5,0.85);"
    "  border: 1px solid rgba(220,60,40,0.55);"
    "  border-radius: 2px;"
    "  padding: 11px 32px;"
    "}"
    ".enc-btn:hover {"
    "  background: rgba(80,10,5,0.9);"
    "  color: #ffb0a0;"
    "  border-color: rgba(255,80,50,0.85);"
    "}"
    ".enc-btn:active {"
    "  background: rgba(15,0,0,0.95);"
    "}"

    ".out-label {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 12px;"
    "  letter-spacing: 3px;"
    "  color: rgba(255,130,80,0.6);"
    "  margin-top: 24px;"
    "  margin-bottom: 6px;"
    "}"
    ".out-box {"
    "  background: rgba(0,0,0,0.7);"
    "  border: 1px solid rgba(180,40,20,0.3);"
    "  border-radius: 2px;"
    "  padding: 12px 16px;"
    "}"
    ".out-text {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  color: #ff8060;"
    "  letter-spacing: 1px;"
    "}"

    ".author {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  color: rgba(180,60,40,0.45);"
    "  letter-spacing: 1px;"
    "}";


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(prov, CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GError *err = NULL;
    bg_pixbuf = gdk_pixbuf_new_from_file("background.jpg", &err);
    if (!bg_pixbuf) {
        g_print("Warning: background.jpg not found: %s\n",
                err ? err->message : "?");
        if (err) g_error_free(err);
    }

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "The Cleaning House");
    gtk_window_set_default_size(GTK_WINDOW(win), 880, 580);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *draw = gtk_drawing_area_new();
    g_signal_connect(draw, "draw", G_CALLBACK(on_draw), NULL);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(overlay), draw);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");
    gtk_widget_set_size_request(card, 560, -1);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);

    GtkWidget *title = gtk_label_new("THE CLEANING HOUSE");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
    gtk_label_set_xalign(GTK_LABEL(title), 0.5);
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(bar), "neon-bar");
    gtk_box_pack_start(GTK_BOX(card), bar, FALSE, FALSE, 0);

    GtkWidget *prompt = gtk_label_new("ENTER YOUR MESSAGE");
    gtk_style_context_add_class(gtk_widget_get_style_context(prompt), "prompt");
    gtk_label_set_xalign(GTK_LABEL(prompt), 0.0);
    gtk_box_pack_start(GTK_BOX(card), prompt, FALSE, FALSE, 0);

    entry_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_input), "type your string here...");
    gtk_style_context_add_class(gtk_widget_get_style_context(entry_input), "input-box");
    g_signal_connect(entry_input, "activate", G_CALLBACK(on_enter), NULL);
    gtk_box_pack_start(GTK_BOX(card), entry_input, FALSE, FALSE, 8);

    GtkWidget *btn = gtk_button_new_with_label("ENCRYPT");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "enc-btn");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_encrypt), NULL);
    gtk_box_pack_start(GTK_BOX(card), btn, FALSE, FALSE, 4);

    GtkWidget *out_lbl = gtk_label_new("YOUR GIFT:");
    gtk_style_context_add_class(gtk_widget_get_style_context(out_lbl), "out-label");
    gtk_label_set_xalign(GTK_LABEL(out_lbl), 0.0);
    gtk_box_pack_start(GTK_BOX(card), out_lbl, FALSE, FALSE, 0);

    GtkWidget *out_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(out_box), "out-box");
    gtk_box_pack_start(GTK_BOX(card), out_box, FALSE, FALSE, 0);

    lbl_output = gtk_label_new("—");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_output), "out-text");
    gtk_label_set_xalign(GTK_LABEL(lbl_output), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(lbl_output), TRUE);
    gtk_label_set_selectable(GTK_LABEL(lbl_output), TRUE);
    gtk_box_pack_start(GTK_BOX(out_box), lbl_output, FALSE, FALSE, 0);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), card);

    GtkWidget *author = gtk_label_new("Author : OTC");
    gtk_style_context_add_class(gtk_widget_get_style_context(author), "author");
    gtk_widget_set_valign(author, GTK_ALIGN_END);
    gtk_widget_set_halign(author, GTK_ALIGN_START);
    gtk_widget_set_margin_start(author, 14);
    gtk_widget_set_margin_bottom(author, 10);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), author);

    gtk_container_add(GTK_CONTAINER(win), overlay);
    gtk_widget_show_all(win);

    gtk_main();
    return 0;
}