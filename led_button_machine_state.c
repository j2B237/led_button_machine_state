#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>


// ---- Definition des broches -------
#define LED_PIN PB2
#define BUTTON_PIN PB0

// ---- definition des structures -----
typedef struct StateMachine StateMachine;

typedef struct State {
    void (*enter)(StateMachine *sm);
    void (*handle)(StateMachine *sm);
    void (*exit) (StateMachine *sm);
}State;


typedef struct StateMachine {
    State *currentState;
    bool buttonWasPressed;
}StateMachine;


// --- Déclaration des états et des fonctions de transition ---
void transitionTo(StateMachine* sm, State* nextState);

void enter_led_off(StateMachine* sm);
void handle_led_off(StateMachine* sm);
void exit_led_off(StateMachine* sm);

void enter_led_on(StateMachine* sm);
void handle_led_on(StateMachine* sm);
void exit_led_on(StateMachine* sm);


// --- Définition des objets d'état ---
State ledOffState = {
    .enter = &enter_led_off,
    .handle = &handle_led_off,
    .exit = &exit_led_off
};

State ledOnState = {
    .enter = &enter_led_on,
    .handle = &handle_led_on,
    .exit = &exit_led_on
};

// --- Fonctions de transition d'état ---
void transitionTo(StateMachine* sm, State* nextState) {
    if (sm->currentState && sm->currentState->exit) {
        sm->currentState->exit(sm);
    }
    
    sm->currentState = nextState;
    
    if (sm->currentState->enter) {
        sm->currentState->enter(sm);
    }
}

// --- Fonctions pour l'état LED_OFF ---
void enter_led_off(StateMachine* sm) {
    // Éteindre la LED en mettant la broche à LOW
    PORTB &= ~(1 << LED_PIN); 
}

void handle_led_off(StateMachine* sm) {
    if (sm->buttonWasPressed) {
        transitionTo(sm, &ledOnState);
    }
}

void exit_led_off(StateMachine* sm) {
    // Aucune action spécifique
}

// --- Fonctions pour l'état LED_ON ---
void enter_led_on(StateMachine* sm) {
    // Allumer la LED en mettant la broche à HIGH
    PORTB |= (1 << LED_PIN);
}

void handle_led_on(StateMachine* sm) {
    if (sm->buttonWasPressed) {
        transitionTo(sm, &ledOffState);
    }
}

void exit_led_on(StateMachine* sm) {
    // Aucune action spécifique
}



// --- Fonction principale ---
int main(void) {

    // Configuration des broches
    DDRB |= (1 << LED_PIN); // Mettre la broche de la LED en sortie
    DDRB &= ~(1 << BUTTON_PIN); // Mettre la broche du bouton en entrée 
    PORTB |= (1 << BUTTON_PIN); // Activer la résistance de pull-up interne du bouton

    StateMachine myMachine;
    myMachine.buttonWasPressed = false;
    
    bool buttonState = false;
    bool lastButtonState = false;

    // Démarrer la machine dans l'état initial
    transitionTo(&myMachine, &ledOffState);

    // Event loop

    while (1) {
        // Lecture du bouton (active-bas)
        buttonState = !(PINB & (1 << BUTTON_PIN));
        
        // Détection d'un appui unique
        if (buttonState && !lastButtonState) {
            myMachine.buttonWasPressed = true;
        } else {
            myMachine.buttonWasPressed = false;
        }
        
        lastButtonState = buttonState;

        // Appeler la fonction handle de l'état courant
        if (myMachine.currentState && myMachine.currentState->handle) {
            myMachine.currentState->handle(&myMachine);
        }

        _delay_ms(10); // Petit délai anti-rebond
    }

    return 0;
}