# Télécharger sur Telegram :
<h2><a href="[https://t.me/k5robby69](https://t.me/k5robby69)">🗲 Telegram </a></h2>

# Documentation Firmware Robzyl K5 (V1)

## Les modes VFO et Mémoire

Ces modes sont accessibles alternativement par un appui long sur la touche 3. Sur ces écrans, les touches 4/5/6/0 en appui long permettent de régler BANDWIDTH/STEP/POWER/MODULATION. Le menu en touche M donne également accès à tous ces paramètres. En réception, il apparaît un minuteur de temps d'écoute.

### VFO MODE

<img width="512" height="320" alt="2-VFO" src="https://github.com/user-attachments/assets/b22aa54a-6bd5-4d00-9600-564f20f88325" />

Le mode simple VFO permet de saisir librement une fréquence. En réception, il apparaît à l'écran la valeur d'AFC (info de décalage de fréquence), la valeur du S-mètre et la puissance du signal en dBm.

### MR MODE (Mémoires)

<img width="512" height="320" alt="1-MR" src="https://github.com/user-attachments/assets/069a11e4-e81d-4670-9d12-cf08a2ac43df" />

Cet autre mode permet de naviguer dans la banque des 200 mémoires nommées du K5 8K (ou 999 pour le K5 512K). Cette banque est à préparer et à injecter dans la radio depuis Chirp (driver 8k ou 512k). En réception, il apparaît à l'écran la valeur du S-mètre et la puissance du signal en dBm.

## Le mode spectre

### Fonctionnalités communes du mode spectre

Écran principal :

<img width="512" height="320" alt="3-spectre vue 1" src="https://github.com/user-attachments/assets/f7f1c0d7-538a-4fbd-8bb2-68a64e8bea8a" />

* Ligne haute :
* Type de spectre : SL (Scan Lists), FQ (Fréquences avoisinantes au VFO), BD (Bandes => le code bande pays apparaît en 1er), RG (Plage si définie via le menu).
* Valeur trigger UP Uxx du squelch (valeur de déclenchement sur signal montant).
* Délai de capture du RSSI d’un signal allant de 0 à 12 ms. Permet d’accélérer la vitesse de scan, mais cela réduit le rapport signal sur bruit.
* Modulation courante FM/AM/USB.
* Niveau de la batterie.


* Cadre 1 : 2 affichages possibles des informations liées à la fréquence en cours en pressant 8.
* Cadre 2 : Représentation graphique et dynamique des canaux analysés avec leur niveau de signal.
* Ligne basse : Étendue en cours et pic de fréquence au centre.

### Affectation des touches

* Touche 1 : Passer une fréquence à l’écoute (« skip »).
* Touche 2 : Accès à l’écran simplifié façon scanner simple.
* Touche 3 : Sélection de la largeur de bande d’écoute.
* Touche 4 : Menu de choix mono ou multiple SL/BD.
* Touche 5 : Accès aux Paramètres, puis ^/v pour naviguer, 1/3 pour changer les valeurs, 1/M pour saisir Fstart/Fstop (RG mode).
* Touche 6 : Navigation dans les modes SL/BD/FQ.
* Touche 7 : Sauvegarde des principaux paramètres.
* Touche 8 : 2 options d'affichage :
* Fréquence seule grande taille + info canal.
* Fréquence + info canal + timer d'écoute (petite taille).
* Touche 9 : Choix de la modulation.
* Touche 0 : Accès à l’écran d'historique.
* Touche M : Passage en Still mode (monitoring et accès registres).
* SIDE KEY 1 : Passer du mode Normal à FL (verrouillage de fréquence puis Monitor) et à M (écoute ouverte).
* SIDE KEY 2 : Blacklister une fréquence à l’écoute.
* Touche */F : Réglage squelch paramètre Uxx. 
  Si Uoo, enregistrment de l'historique sans s'arrêter
* Touche ^/v : Naviguer dans les SL ou les bandes.

### Menu des paramètres

<img width="512" height="320" alt="4-Menu" src="https://github.com/user-attachments/assets/752c4f6d-6626-4798-9886-c95ca67700de" />

* RSSI Delay : temps de capture du RSSI en ms. Une valeur trop faible peut faire rater des signaux.
* SpectrumDelay : Permet de définir le temps d’attente sur un signal à l’écoute ayant chuté sous le squelch. Si la valeur est à l’infini : pressez la touche Exit pour quitter l’écran d’écoute.
* Max Listen Time : temps maximum d'écoute d'une fréquence reçue.
* PTT (Option de passage en émission) : LAST RECEIVED = dernière fréquence entendue, LAST VFO FREQ = fréquence en VFO, NINJA MODE : Mode de communication expérimental par saut de fréquence à chaque PTT entre deux K5 utilisant le spectre en mode Ninja sur une Scanlist commune. Voir vidéo sur YouTube.
* Fstart/Fstop : paramétrage des fréquences ^/v (mode RG).
* Step : paramétrage de la canalisation des fréquences.
* ListenBW : paramétrage de la largeur de la bande d’écoute.
* Modulation : FM/AM/USB.
* DEFAUT PARAMS et touche 3 pour réinitialiser les paramètres du spectre ainsi que les registres.
* RX_Backlight_ON : permet d'activer le rétroéclairage en réception spectre.
* Freq counting : l'historique compte le nombre de réceptions d'une fréquence, Time counting compte le temps écoulé en réception de la fréquence.
* CLEAR HISTORY : efface l'historique de l'EEPROM (version 512k).
* FREE RAM : indique la mémoire disponible.
* PowerSave : permet d'augmenter le délai de réactualisation du spectre sur l'écran LCD.
* Noislvl_OFF : permet d'ajuster le niveau de bruit à prendre en compte pour éviter des déclenchements d'écoutes intempestifs.
* POPUPS : règle le délai d'affichage des messages en surimpression.
* U00_trigger : voir ci-dessous Historique.

### Vue simplifiée

<img width="512" height="320" alt="5-spectre simplifé" src="https://github.com/user-attachments/assets/cf74343c-cc15-4a25-95e4-eca36844a643" />

Cet écran offre une vue plus synthétique du scan en cours tout en permettant le réglage aisé des paramètres de squelch.

### Still mode (monitoring de fréquence)

<img width="512" height="320" alt="6-Mode still" src="https://github.com/user-attachments/assets/6c8e06e8-dabb-450f-b95a-aeac241ed38a" />

Le monitor se lance avec la touche M sur une fréquence en écoute. Sur cet écran, certains registres sont modifiables pour les utilisateurs avancés.

### Historique des fréquences

<img width="512" height="320" alt="7-Historique" src="https://github.com/user-attachments/assets/f9090de3-a594-4e61-a2d6-7b41627531e8" />

L'historique évolue dynamiquement au gré des fréquences reçues. Il est possible de naviguer dans la liste, la radio passe en Frequency Lock (FL) et on peut écouter directement les fréquences stockées (comme une radio FM qui balaie et enregistre des fréquences).

* Touche M : passer en Frequency Lock puis monitoring sur la fréquence. Touche PTT pour copier la fréquence vers le mode VFO.
* Touche 2 : sauvegarder l'entrée d'historique sélectionnée dans la première mémoire disponible.
* Touche 3 : effacer l'entrée de l'historique.
* Touche 5 : scanner les entrées de l'historique.
* Touche 7 : sauvegarder l'historique en EEPROM (version 512k).
* Touche 8 : effacer l'historique en mémoire, mais pas en EEPROM.

Il existe un mode spécial de scan en valeur U00 (juste avant la valeur U0). Ce mode permet de collecter très rapidement un historique sans s'arrêter en écoute ; c'est le paramètre U00_trigger du menu du spectre qui permet d'ajuster un niveau seuil de déclenchement des signaux à historiser.

### Conseils

* La valeur de réglage du squelch dépend de votre environnement, de votre antenne et de votre choix de délai RSSI.
* RSSI Delay : commencer par exemple à 3 ms et ajuster jusqu'à obtenir un bon compromis entre vitesse et réception.
* Trigger Up Uxxx : commencer par exemple à 5 et ajuster jusqu’à ne plus recevoir de bruits.
* Noise level (Noislvl) : commencer par exemple à 60 et ajuster pour limiter les faux signaux.

## Spectre sur les ScanLists (mode SL)

* Fonction : Permet de charger dans le spectre les mémoires affectées à des scanlists.
* Lancement : Depuis le mode VFO/MR, touche F+4.
* Utilisation et conseils :
* Préalablement, les fréquences en mémoires doivent avoir été affectées à une scanlist (ex. SL1 = PMR, SL2 = Répéteurs, SL3 = Aéro, etc.).
* À la première utilisation, vous pouvez naviguer dans chaque SL (^/v) pour ajuster les paramètres de squelch U, puis mémoriser vos valeurs avec la touche 7.
* Enfin, chargez vos SL dans le spectre via le menu de sélection en touche 4.



<img width="512" height="320" alt="8-SL sel menu" src="https://github.com/user-attachments/assets/1f7e690b-b5b3-4cdd-a84d-899616d78f8e" />

On navigue dans ce menu avec les touches ^/v.

* Touche 5 : choisir une SL en excluant les autres.
* Touche 4 : choisir/invalider une ou plusieurs SL.
* Touche * : affichage des mémoires affectées à la SL sélectionnée.

Les SL choisies apparaissent avec un symbole *. Puis faire Exit pour lancer le spectre. Touche 7 pour enregistrer sa configuration.

# Spectre sur plage de fréquences (mode FQ) :

* Fonction : Permet d’analyser une gamme de fréquences à partir d’une fréquence centrale ou bien à partir d’une étendue définie.
* Lancement : Depuis le mode VFO/MR, touche F+5.
* Utilisation et conseils :
* La fréquence issue du VFO/MR est portée au spectre en tant que fréquence centrale. Ensuite, vous pouvez agir sur le paramétrage de votre spectre selon vos besoins en step, modulation, etc. (Réglages touche 5).
* L’étendue des fréquences basse/haute peut être ajustée dans le menu via les paramètres FStart/FStop. Sur ces paramètres, faire 1 pour accéder à la saisie et M pour valider (touche * pour la virgule).
* Ajuster votre squelch.



## Spectre sur les bandes prédéfinies (mode BD)

* Fonction : Permet d’analyser en spectre des bandes prédéfinies (ex. PMR, CB, AERO, HAM, etc.).
* Lancement : Depuis le mode VFO/MR, touche F+6.

## Puissances

* Low : puissances situées auour de 1W selon les bandes VHF ou UHF.
* Low VERSION DEV:puissance de quelques milliwatts, convient pour faire des tests de proximité entre radios.
* Mid : puissances situées entre 2 et 3W selon les bandes VHF ou UHF.
* High : puissance maximale proposée par le matériel, soit en moyenne 5W.

<h2><a href="[https://www.youtube.com/@robby_69400](https://www.youtube.com/@robby_69400)" rel="nofollow">🗲 Youtube</a></h2>

### 🙏 Many thanks to Zylka, Kolyan, Iggy, Toni, Yves and Francois
