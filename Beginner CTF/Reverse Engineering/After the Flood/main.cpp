#include <gtk/gtk.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdint>
#include <ctime>
#include <string>

#define WIN_GOAL 100000

#define ROL8(x,r) ((uint8_t)((((x)<<((r)&7))|((x)>>(8-((r)&7))))&0xFF))
#define ROR8(x,r) ((uint8_t)((((x)>>((r)&7))|((x)<<(8-((r)&7))))&0xFF))

static const uint8_t C_EVEN[31] = {
    0xEA,0x45,0x09,0x95,0xF9,0x3E,0x1D,0x38,0xFA,0x66,0xAA,
    0x2F,0x6D,0x0C,0x9B,0xDD,0x9F,0x04,0xDE,0x96,0xCE,0xE7,
    0x8B,0xE4,0x81,0x54,0x48,0x9A,0x70,0x4F,0x4B
};
static const uint8_t C_ODD[31] = {
    0x9E,0x15,0x33,0xC3,0x36,0xFB,0x19,0x3E,0x6D,0xC1,0xA9,
    0x23,0x07,0xE7,0x01,0xCE,0x24,0x71,0xF1,0x06,0xF4,0x01,
    0x31,0x84,0xFF,0x25,0xEC,0xB4,0x94,0x5B,0xEF
};
static const uint8_t K[6] = { 0xA9,0x13,0x7C,0x21,0x5E,0xC3 };

static int total_wins   = 0;
static int cur_target   = 0;
static int cur_attempts = 5;

static std::string decrypt_flag() {
    uint8_t enc[62];
    for (int i=0,e=0,o=0; i<62; ++i)
        enc[i] = (i&1) ? C_ODD[o++] : C_EVEN[e++];
    uint8_t out[63] = {0};
    for (int i=0; i<62; ++i) {
        uint8_t ks = ROR8((uint8_t)(K[i%6]^(uint8_t)i),(uint8_t)(i&7));
        uint8_t x  = (uint8_t)(enc[i]^(uint8_t)((i*29)&0xFF));
        x = ROR8(x,(uint8_t)((i*3)&7));
        out[i] = (uint8_t)(x^ks);
    }
    return std::string((char*)out);
}

static void new_round() {
    cur_target   = rand() % 100 + 1;
    cur_attempts = 5;
}

static GtkWidget *lbl_title_sub;   
static GtkWidget *entry_guess;
static GtkWidget *btn_submit;
static GtkWidget *lbl_feedback;
static GtkWidget *lbl_attempts;
static GtkWidget *lbl_flag;
static GtkWidget *flag_box;
static GdkPixbuf *bg_pixbuf = NULL;

static void update_stats() {
    char buf[64];
    snprintf(buf, sizeof(buf), "ATTEMPTS LEFT  %d", cur_attempts);
    gtk_label_set_text(GTK_LABEL(lbl_attempts), buf);
}

static void on_submit(GtkWidget *, gpointer) {
    const char *raw_txt = gtk_entry_get_text(GTK_ENTRY(entry_guess));
    std::string input = raw_txt ? raw_txt : "";
    gtk_entry_set_text(GTK_ENTRY(entry_guess), "");

    if (input.empty()) {
        gtk_label_set_text(GTK_LABEL(lbl_feedback), "Type a number first.");
        return;
    }
    char *endptr = nullptr;
    long choice_l = strtol(input.c_str(), &endptr, 10);
    while (endptr && *endptr && isspace((unsigned char)*endptr)) {
        endptr++;
    }
    if (endptr == input.c_str() || *endptr != '\0') {
        gtk_label_set_text(GTK_LABEL(lbl_feedback),
                           "Invalid input! Please enter a number.");
        return;
    }
    int choice = (int)choice_l;
    if (choice == 0) {
        gtk_main_quit();
        return;
    }
    if (choice < 1 || choice > 100) {
        gtk_label_set_text(GTK_LABEL(lbl_feedback),
                           "Choose between 1 and 100.");
        return;
    }

    cur_attempts--;

    if (choice == cur_target) {
        total_wins++;
        char fb[128];
        snprintf(fb, sizeof(fb),
                 "Correct!  The number was %d.", cur_target);
        gtk_label_set_text(GTK_LABEL(lbl_feedback), fb);

        if (total_wins >= WIN_GOAL) {
            std::string flag = decrypt_flag();
            gtk_label_set_text(GTK_LABEL(lbl_flag), flag.c_str());
            gtk_widget_show(flag_box);
            gtk_widget_set_sensitive(btn_submit, FALSE);
            gtk_widget_set_sensitive(entry_guess, FALSE);
            update_stats();
            return;
        }
        new_round();

    } else if (cur_attempts <= 0) {
        char fb[128];
        snprintf(fb, sizeof(fb),
                 "Out of attempts. The number was %d.", cur_target);
        gtk_label_set_text(GTK_LABEL(lbl_feedback), fb);
        new_round();

    } else {
        char fb[64];
        snprintf(fb, sizeof(fb),
                 "%s  Try again.",
                 choice < cur_target ? "Too low!" : "Too high!");
        gtk_label_set_text(GTK_LABEL(lbl_feedback), fb);
    }

    update_stats();
}

static void on_enter(GtkWidget *w, gpointer d) { on_submit(w, d); }

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer) {
    GtkAllocation a;
    gtk_widget_get_allocation(widget, &a);

    if (bg_pixbuf) {
        GdkPixbuf *sc = gdk_pixbuf_scale_simple(
            bg_pixbuf, a.width, a.height, GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, sc, 0, 0);
        cairo_paint(cr);
        g_object_unref(sc);
    } else {
        cairo_set_source_rgb(cr, 0.02, 0.04, 0.07);
        cairo_paint(cr);
    }

    cairo_pattern_t *v = cairo_pattern_create_radial(
        a.width*.5, a.height*.6, a.height*.05,
        a.width*.5, a.height*.5, a.height*.9);
    cairo_pattern_add_color_stop_rgba(v, 0.0, 0,0,0, 0.00);
    cairo_pattern_add_color_stop_rgba(v, 0.5, 0,0,0, 0.30);
    cairo_pattern_add_color_stop_rgba(v, 1.0, 0,0,0, 0.88);
    cairo_set_source(cr, v);
    cairo_paint(cr);
    cairo_pattern_destroy(v);

    return FALSE;
}

static const char *CSS =
    "window { background: transparent; }"

    ".card {"
    "  background: rgba(4,10,20,0.80);"
    "  border: 1px solid rgba(60,120,180,0.20);"
    "  border-top: 2px solid rgba(80,160,220,0.50);"
    "  border-radius: 3px;"
    "  padding: 36px 48px 32px 48px;"
    "}"

    ".title {"
    "  font-family: 'Cinzel', 'Palatino Linotype', 'Big Caslon', serif;"
    "  font-size: 48px;"
    "  letter-spacing: 8px;"
    "  color: #a8d4f0;"
    "  text-shadow: 0 0 22px rgba(80,160,230,0.65),"
    "               0 0 48px rgba(40,100,180,0.35);"
    "}"

    ".glow-bar {"
    "  background: linear-gradient(90deg,"
    "    transparent 0%, rgba(60,140,210,0.7) 25%,"
    "    rgba(120,190,240,0.9) 50%,"
    "    rgba(60,140,210,0.7) 75%, transparent 100%);"
    "  min-height: 1px;"
    "  margin: 8px 0 26px 0;"
    "}"

    ".sub {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  letter-spacing: 3px;"
    "  color: rgba(120,180,220,0.60);"
    "  margin-bottom: 10px;"
    "}"

    ".stat {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  letter-spacing: 2px;"
    "  color: rgba(100,170,210,0.55);"
    "}"

    ".guess-entry {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 22px;"
    "  color: #c8e8ff;"
    "  background: rgba(0,8,18,0.82);"
    "  border: 1px solid rgba(60,130,190,0.40);"
    "  border-radius: 2px;"
    "  padding: 10px 18px;"
    "  caret-color: #60b4e8;"
    "}"
    ".guess-entry:focus {"
    "  border-color: rgba(80,170,230,0.85);"
    "  box-shadow: 0 0 0 2px rgba(60,140,210,0.18);"
    "}"

    ".submit-btn {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 12px;"
    "  letter-spacing: 4px;"
    "  color: #80c8f0;"
    "  background: rgba(8,20,40,0.85);"
    "  border: 1px solid rgba(60,140,200,0.50);"
    "  border-radius: 2px;"
    "  padding: 11px 36px;"
    "}"
    ".submit-btn:hover {"
    "  background: rgba(15,40,70,0.92);"
    "  color: #b8e0ff;"
    "  border-color: rgba(80,170,230,0.80);"
    "}"
    ".submit-btn:active { background: rgba(4,12,24,0.95); }"

    ".feedback {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  color: #90c8e8;"
    "  letter-spacing: 1px;"
    "  min-height: 22px;"
    "}"

    ".flag-box {"
    "  background: rgba(0,14,30,0.88);"
    "  border: 1px solid rgba(60,180,120,0.45);"
    "  border-top: 2px solid rgba(60,220,140,0.70);"
    "  border-radius: 2px;"
    "  padding: 14px 18px;"
    "  margin-top: 18px;"
    "}"
    ".flag-lbl {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 13px;"
    "  color: #60e8a0;"
    "  letter-spacing: 1px;"
    "}"

    ".author {"
    "  font-family: 'Courier New', monospace;"
    "  font-size: 11px;"
    "  color: rgba(80,130,170,0.45);"
    "  letter-spacing: 1px;"
    "}";

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));
    new_round();

    gtk_init(&argc, &argv);

    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(prov, CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GError *berr = NULL;
    bg_pixbuf = gdk_pixbuf_new_from_file("background.jpg", &berr);
    if (!bg_pixbuf) {
        g_print("Warning: background.jpg not found: %s\n",
                berr ? berr->message : "?");
        if (berr) g_error_free(berr);
    }

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "After the Flood");
    gtk_window_set_default_size(GTK_WINDOW(win), 900, 600);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *draw = gtk_drawing_area_new();
    g_signal_connect(draw, "draw", G_CALLBACK(on_draw), NULL);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(overlay), draw);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "card");
    gtk_widget_set_size_request(card, 520, -1);
    gtk_widget_set_valign(card, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(card, GTK_ALIGN_CENTER);

    GtkWidget *title = gtk_label_new("AFTER THE FLOOD");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
    gtk_label_set_xalign(GTK_LABEL(title), 0.5);
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    GtkWidget *gbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(gbar), "glow-bar");
    gtk_box_pack_start(GTK_BOX(card), gbar, FALSE, FALSE, 0);

    GtkWidget *sub = gtk_label_new("GUESS A NUMBER BETWEEN 1 AND 100");
    gtk_style_context_add_class(gtk_widget_get_style_context(sub), "sub");
    gtk_label_set_xalign(GTK_LABEL(sub), 0.5);
    gtk_box_pack_start(GTK_BOX(card), sub, FALSE, FALSE, 0);
    lbl_title_sub = sub;

    GtkWidget *stats_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(card), stats_row, FALSE, FALSE, 8);

    lbl_attempts = gtk_label_new("");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_attempts), "stat");
    gtk_label_set_xalign(GTK_LABEL(lbl_attempts), 0.5);
    gtk_box_pack_start(GTK_BOX(stats_row), lbl_attempts, TRUE, TRUE, 0);

    entry_guess = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_guess), "0 to quit");

    gtk_style_context_add_class(gtk_widget_get_style_context(entry_guess), "guess-entry");
    g_signal_connect(entry_guess, "activate", G_CALLBACK(on_enter), NULL);
    gtk_box_pack_start(GTK_BOX(card), entry_guess, FALSE, FALSE, 6);

    btn_submit = gtk_button_new_with_label("SUBMIT");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn_submit), "submit-btn");
    g_signal_connect(btn_submit, "clicked", G_CALLBACK(on_submit), NULL);
    gtk_box_pack_start(GTK_BOX(card), btn_submit, FALSE, FALSE, 4);

    lbl_feedback = gtk_label_new("—");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_feedback), "feedback");
    gtk_label_set_xalign(GTK_LABEL(lbl_feedback), 0.5);
    gtk_box_pack_start(GTK_BOX(card), lbl_feedback, FALSE, FALSE, 10);

    flag_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(flag_box), "flag-box");

    GtkWidget *flag_hdr = gtk_label_new("Congratulations! Here is your flag:");
    gtk_style_context_add_class(gtk_widget_get_style_context(flag_hdr), "sub");
    gtk_label_set_xalign(GTK_LABEL(flag_hdr), 0.5);
    gtk_box_pack_start(GTK_BOX(flag_box), flag_hdr, FALSE, FALSE, 0);

    lbl_flag = gtk_label_new("");
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl_flag), "flag-lbl");
    gtk_label_set_xalign(GTK_LABEL(lbl_flag), 0.5);
    gtk_label_set_selectable(GTK_LABEL(lbl_flag), TRUE);
    gtk_box_pack_start(GTK_BOX(flag_box), lbl_flag, FALSE, FALSE, 8);

    gtk_box_pack_start(GTK_BOX(card), flag_box, FALSE, FALSE, 0);
    gtk_widget_hide(flag_box);

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
    gtk_widget_hide(flag_box);

    update_stats();
    gtk_main();
    return 0;
}