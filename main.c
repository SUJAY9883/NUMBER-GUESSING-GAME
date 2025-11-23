#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- CSS FOR STYLING (GTK4 SAFE) ---
const char *css_data =
    "window { background-color: #f0f2f5; }"
    ".card { background-color: white; border-radius: 15px; padding: 20px;"
    "margin: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
    ".header-title { font-size: 25px; font-weight: bold; color: #3b5998; margin-bottom: 10px; }"
    ".name-question { font-size: 18px; font-weight: 500; color: #444; margin-bottom: 15px; }"
    ".welcome-text { font-size: 28px; font-weight: 800; color: #333; margin-bottom: 20px; }"
    ".input-field { font-size: 18px; padding: 10px; margin-bottom: 20px; border-radius: 8px; }"
    ".guess-cheer-text { font-size: 18px; font-weight: 600; color: #444; }"
    ".btn-green { background-color: #4caf50; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
    ".btn-blue { background-color: #3b82f6; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
    ".btn-red { background-color: #ef4444; color: white; font-weight: bold; padding: 10px; border-radius: 8px; }"
    ".feedback-box { background-color: #fee2e2; color: #991b1b; border-radius: 8px; padding: 10px; margin-top: 15px; font-weight: bold; }"
    ".success-text { font-size: 20px; font-weight: bold; color: #1e293b; }"
    ".big-number { font-size: 25px; font-weight: 900; color: #22c55e; margin: 10px; }"
    ".tip-text { color: #666; font-size: 15px; margin-top: 20px; }"
    ".warning-text { color: #ff3333; font-weight: 700; font-size: 15px; margin-top: 5px; }"
    ".attempts-text { color: #0f3edd; font-size: 20px; font-weight: 700; }";

// --- GAME STRUCT ---
typedef struct
{
    GtkWidget *stack;
    GtkWidget *window;

    GtkWidget *name_entry;
    GtkWidget *name_warning_label;   // << NEW warning label

    GtkWidget *greeting_label;
    GtkWidget *guess_spin;
    GtkWidget *feedback_label;
    GtkWidget *feedback_revealer;
    GtkWidget *attempts_label;

    GtkWidget *congrats_label;
    GtkWidget *secret_number_label;
    GtkWidget *total_attempts_label;
    GtkWidget *praise_label;

    int secret_number;
    int attempts;
    char player_name[100];

} GameApp;

// --- LOGIC FUNCTIONS ---
static void start_game_logic(GameApp *app)
{
    srand(time(0));
    app->secret_number = (rand() % 100) + 1;
    app->attempts = 0;

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->guess_spin), 0);
    gtk_label_set_text(GTK_LABEL(app->feedback_label), "");
    gtk_widget_set_visible(app->feedback_revealer, FALSE);
    gtk_label_set_text(GTK_LABEL(app->attempts_label), "Attempts: 0");

    char greet[200];
    snprintf(greet, sizeof(greet),
             "Welcome, %s!\nI'm thinking of a secret number (1–100).\nCan you read my mind?",
             app->player_name);

    gtk_label_set_text(GTK_LABEL(app->greeting_label), greet);
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_game");
}

static void on_start_clicked(GtkButton *btn, GameApp *app)
{
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app->name_entry));

    if (strlen(name) == 0)
    {
        gtk_label_set_text(GTK_LABEL(app->name_warning_label),
                           "Whoops! I need a name to cheer for. Please enter one!");
        gtk_widget_set_visible(app->name_warning_label, TRUE);
        return;
    }

    gtk_widget_set_visible(app->name_warning_label, FALSE);
    strncpy(app->player_name, name, 99);

    start_game_logic(app);
}

static void on_submit_guess(GtkButton *btn, GameApp *app)
{
    int guess = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->guess_spin));

    if (guess < 1 || guess > 100)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label),
                           "Whoa there! That's not in the rulebook.\nOnly numbers from 1 to 100, please!");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
        return;
    }

    app->attempts++;

    char attempts_text[50];
    snprintf(attempts_text, sizeof(attempts_text), "Attempts: %d", app->attempts);
    gtk_label_set_text(GTK_LABEL(app->attempts_label), attempts_text);

    if (guess > app->secret_number)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label),
                           "Overshot it! Try a smaller number.");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
    }
    else if (guess < app->secret_number)
    {
        gtk_label_set_text(GTK_LABEL(app->feedback_label),
                           "That's too low! Aim a little higher!");
        gtk_widget_set_visible(app->feedback_revealer, TRUE);
    }
    else
    {
        char congrats[200], numbuf[64], attempts_final[64];

        snprintf(congrats, sizeof(congrats), "Bullseye, %s!", app->player_name);
        snprintf(numbuf, sizeof(numbuf), "The secret number was %d!", app->secret_number);
        snprintf(attempts_final, sizeof(attempts_final), "You cracked it in %d guesses!", app->attempts);

        gtk_label_set_text(GTK_LABEL(app->congrats_label), congrats);
        gtk_label_set_text(GTK_LABEL(app->secret_number_label), numbuf);
        gtk_label_set_text(GTK_LABEL(app->total_attempts_label), attempts_final);

        if (app->attempts == 1)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "IMPOSSIBLE!!! Are you psychic?!");
        else if (app->attempts <= 4)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Mastermind! Absolute brilliance!");
        else if (app->attempts <= 7)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Amazing job! You're sharp!");
        else if (app->attempts <= 10)
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Well done! You kept going strong!");
        else
            gtk_label_set_text(GTK_LABEL(app->praise_label), "Phew! You got it just in time!");

        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "page_result");
    }
}

static void on_play_again_clicked(GtkButton *btn, GameApp *app)
{
    start_game_logic(app);
}

static void on_exit_clicked(GtkButton *btn, GameApp *app)
{
    g_application_quit(G_APPLICATION(gtk_window_get_application(GTK_WINDOW(app->window))));
}

// --- UI HELPERS ---
GtkWidget *create_card_box()
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_add_css_class(box, "card");
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(box, 320, -1);
    return box;
}

// --- PAGE BUILDERS ---
GtkWidget *create_welcome_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *header = gtk_label_new("🎮 NUMBER GUESSING GAME");
    gtk_widget_add_css_class(header, "header-title");

    GtkWidget *welcome = gtk_label_new("Welcome!");
    gtk_widget_add_css_class(welcome, "welcome-text");

    GtkWidget *instruction =
        gtk_label_new("What should I call you, challenger?");
    gtk_widget_add_css_class(instruction, "name-question");

    app->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->name_entry),
                                   "Enter your legendary name...");
    gtk_widget_add_css_class(app->name_entry, "input-field");

    // NEW WARNING LABEL BELOW THE BUTTON
    app->name_warning_label = gtk_label_new("");
    gtk_widget_add_css_class(app->name_warning_label, "warning-text");
    gtk_widget_set_visible(app->name_warning_label, FALSE);

    GtkWidget *start_btn = gtk_button_new_with_label("START");
    gtk_widget_add_css_class(start_btn, "btn-green");
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), app);

    GtkWidget *tip = gtk_label_new(
        "💡 Tip: Hey hero… don’t forget to tell me who you are!");
    gtk_widget_add_css_class(tip, "tip-text");

    gtk_box_append(GTK_BOX(box), header);
    gtk_box_append(GTK_BOX(box), welcome);
    gtk_box_append(GTK_BOX(box), instruction);
    gtk_box_append(GTK_BOX(box), app->name_entry);
    gtk_box_append(GTK_BOX(box), start_btn);
    gtk_box_append(GTK_BOX(box), app->name_warning_label); // << added
    gtk_box_append(GTK_BOX(box), tip);

    return box;
}

GtkWidget *create_game_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    GtkWidget *header = gtk_label_new("🎮 NUMBER GUESSING GAME");
    gtk_widget_add_css_class(header, "header-title");

    app->greeting_label = gtk_label_new("Hello, Player!");
    gtk_widget_add_css_class(app->greeting_label, "success-text");

    GtkWidget *instruct = gtk_label_new(
        "What's your first guess?\nTake your best shot!");
    gtk_widget_add_css_class(instruct, "guess-cheer-text");

    app->guess_spin = gtk_spin_button_new_with_range(0, 150, 1);
    gtk_widget_add_css_class(app->guess_spin, "input-field");
    gtk_widget_set_halign(app->guess_spin, GTK_ALIGN_CENTER);

    GtkWidget *submit_btn = gtk_button_new_with_label("SUBMIT GUESS");
    gtk_widget_add_css_class(submit_btn, "btn-blue");
    g_signal_connect(submit_btn, "clicked", G_CALLBACK(on_submit_guess), app);

    app->attempts_label = gtk_label_new("Attempts: 0");
    gtk_widget_add_css_class(app->attempts_label, "attempts-text"); // << NEW CLASS

    app->feedback_revealer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    app->feedback_label = gtk_label_new("");
    gtk_widget_add_css_class(app->feedback_revealer, "feedback-box");
    gtk_box_append(GTK_BOX(app->feedback_revealer), app->feedback_label);
    gtk_widget_set_visible(app->feedback_revealer, FALSE);

    gtk_box_append(GTK_BOX(box), header);
    gtk_box_append(GTK_BOX(box), app->greeting_label);
    gtk_box_append(GTK_BOX(box), instruct);
    gtk_box_append(GTK_BOX(box), app->guess_spin);
    gtk_box_append(GTK_BOX(box), submit_btn);
    gtk_box_append(GTK_BOX(box), app->attempts_label);
    gtk_box_append(GTK_BOX(box), app->feedback_revealer);

    return box;
}

GtkWidget *create_result_page(GameApp *app)
{
    GtkWidget *box = create_card_box();

    app->congrats_label = gtk_label_new("CONGRATULATIONS!");
    gtk_widget_add_css_class(app->congrats_label, "header-title");

    GtkWidget *txt1 = gtk_label_new("You guessed the number:");

    app->secret_number_label = gtk_label_new("0");
    gtk_widget_add_css_class(app->secret_number_label, "big-number");

    app->total_attempts_label = gtk_label_new("Total Attempts: 0");
    gtk_widget_add_css_class(app->total_attempts_label, "success-text");

    app->praise_label = gtk_label_new("");

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    GtkWidget *play_btn = gtk_button_new_with_label("PLAY AGAIN");
    gtk_widget_add_css_class(play_btn, "btn-blue");
    g_signal_connect(play_btn, "clicked", G_CALLBACK(on_play_again_clicked), app);

    GtkWidget *exit_btn = gtk_button_new_with_label("EXIT GAME");
    gtk_widget_add_css_class(exit_btn, "btn-red");
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(on_exit_clicked), app);

    gtk_box_append(GTK_BOX(btn_box), play_btn);
    gtk_box_append(GTK_BOX(btn_box), exit_btn);

    gtk_box_append(GTK_BOX(box), app->congrats_label);
    gtk_box_append(GTK_BOX(box), txt1);
    gtk_box_append(GTK_BOX(box), app->secret_number_label);
    gtk_box_append(GTK_BOX(box), app->total_attempts_label);
    gtk_box_append(GTK_BOX(box), app->praise_label);
    gtk_box_append(GTK_BOX(box), btn_box);

    return box;
}

// --- MAIN GTK APP ---
static void activate(GtkApplication *app_system, gpointer user_data)
{
    GameApp *app = g_new0(GameApp, 1);

    app->window = gtk_application_window_new(app_system);
    gtk_window_set_title(GTK_WINDOW(app->window), "Guess The Number");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 400, 500);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css_data);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack),
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    GtkWidget *page1 = create_welcome_page(app);
    GtkWidget *page2 = create_game_page(app);
    GtkWidget *page3 = create_result_page(app);

    gtk_stack_add_named(GTK_STACK(app->stack), page1, "page_welcome");
    gtk_stack_add_named(GTK_STACK(app->stack), page2, "page_game");
    gtk_stack_add_named(GTK_STACK(app->stack), page3, "page_result");

    gtk_window_set_child(GTK_WINDOW(app->window), app->stack);
    gtk_window_present(GTK_WINDOW(app->window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.numberguess",
                              G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
