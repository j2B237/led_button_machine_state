# State machine

Une machine à états est un modèle de calcul qui peut être dans un seul état à la fois. Elle passe d'un état à l'autre en réponse à des événements ou des conditions.
Cette structure stocke directement un pointeur vers la structure State (état courant), simplifiant ainsi la gestion des pointeurs.

## State

La déclaration typedef struct State { ... } State; crée une structure nommée State qui sert de "moule" pour définir un état. 
Chaque état de votre machine aura ce même "moule" ou "format".
Cette structure contient trois pointeurs de fonction. Un pointeur de fonction est une variable qui stocke l'adresse d'une fonction, 
permettant d'appeler cette fonction indirectement. C'est un concept puissant en C pour créer des comportements dynamiques.

Voici le détail de chaque pointeur de fonction à l'intérieur de la structure :

1. void (*enter)(StateMachine* sm);
    enter est le nom du pointeur de fonction.
    void (*enter) signifie que la fonction pointée ne retourne rien (void).
    (StateMachine* sm) signifie que la fonction pointée prend en argument un pointeur vers une autre structure, StateMachine.
    C'est essentiel, car les fonctions de l'état (entrer, gérer, sortir) ont besoin de pouvoir manipuler l'état global de la machine.


    Rôle : Cette fonction est appelée une seule fois lorsque la machine entre dans cet état. Elle est utilisée pour initialiser les variables 
    ou exécuter les actions nécessaires au moment de l'entrée dans l'état (par exemple, allumer une LED, démarrer un timer, afficher un message de démarrage).


2. void (*handle)(StateMachine* sm);
    handle est le nom du pointeur de fonction.
    void (*handle) signifie que la fonction pointée ne retourne rien (void).
    (StateMachine* sm) signifie qu'elle prend en argument un pointeur vers la machine à états.

     Rôle: Cette fonction est appelée de manière répétée tant que la machine est dans cet état. Elle gère la logique principale de l'état, vérifie les conditions
     vers d'autres états et exécute les actions continues. C'est souvent la fonction qui se trouve dans la boucle principale et qui est appelée à chaque itération.


 3. void (*exit)(StateMachine* sm);
      exit est le nom du pointeur de fonction.
      void (*exit) signifie que le fonction pointée ne retourne rien (void).
      (StateMachine *sm) signifie qu'elle prend en argument un pointeur vers la machine à états.

      Rôle : Cette fonction est appélée une seule fois lorsque la machine quitte cet état pour passer à un autre. Elle est utilisée pour faire le nettoyage ou exécuter
      les actions de fin d'état (par exemple éteindre la led, arreter un timer, désallouer de la mémoire).

Le programme en C présente une machine à états à deux états (LED_OFF, lED_ON). L'état de la machine basculera de LED_OFF à LED_ON quand le bouton est préssé et de LED_ON à LED_OFF
quand le bouton est préssé de nouveau.

Afin de gérer ces deux états nous avons des fonctions d'états 

## Fonction d'états

Chaque état a ses propres fonctions enter, handle et exit qui sont implémentées séparemment.

      void enter_led_off(StateMachine* sm);
      void handle_led_off(StateMachine* sm);
      void exit_led_off(StateMachine* sm);
      
      void enter_led_on(StateMachine* sm);
      void handle_led_on(StateMachine* sm);
      void exit_led_on(StateMachine* sm);


## Les objets d'états

Les objets d'états (ledOffState et ledOnState) sont des instances de la structure State. Ils permettent de gérer l'éxécution de la machine lors de la transition d'un état vers un autre.
A chaque objet une assignation par adresse de fonctions d'état correspondante doit etre faite.

## Fonction de transition

tansitionTo() est cruciale. Elle est responsable de :
    * Appeler la fonction exit de l'état courant (si elle existe).
    * Mettre à jour les pointeurs de l'état courant de la structure StateMachine pour qu'ils pointent vers les fonction du nouvel état.
    * Appele la fonction enter du nouvel état.


La fonction main(), permet de : 

   * configurer les broches du MCU (Atmega328p)
   * Déclarer un objet de type StateMachine
   * Appeler la fonction de transition vers l'état ledOffState

La boucle principale de la fonction main () procéde aux instructions suivantes : 

  * vérifie que le bouton est appuyé (active-bas)
  * Appelle la fonction handle de l'état courant stocké dans l'objet de type StateMachine.

