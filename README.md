# Système de Communication GSM EcoCar

## Aperçu
Ce projet est un système de télécommunication basé sur GSM conçu pour l’équipe EcoCar participant au Shell Eco Marathon. Il permet une communication en temps réel entre le pilote dans le véhicule et l’équipe à l’extérieur à l’aide d’un microcontrôleur ESP32, d’un module GSM SIM800L, de deux boutons, d’un microphone, d’un haut-parleur et d’un écran OLED. Le système permet au pilote d’initier des appels, de recevoir automatiquement les appels de l’équipe, de rejeter les appels inconnus et d’afficher les statuts sur l’écran OLED.

## Fonctionnalités
- **Appels initiés par le pilote** : Le pilote peut appuyer sur un bouton vert pour appeler un numéro d’équipe prédéfini.
- **Gestion automatique des appels** : Les appels entrants de l’équipe sont automatiquement pris, tandis que les appels de numéros inconnus sont rejetés avec un message affiché.
- **Fin d’appel** : Le pilote peut terminer l’appel avec un bouton rouge, ou l’équipe peut raccrocher, avec un affichage correspondant sur l’écran OLED.
- **Affichage en temps réel** : Un écran OLED 128x64 affiche les statuts du système (par exemple, « Système prêt », « Appel lancé », « Appel terminé »).
- **Configuration audio** : Utilise un microphone et un haut-parleur (provenant d’écouteurs) pour une communication bidirectionnelle, avec réglage automatique du volume et du canal audio.
- **Suivi du réseau** : Affiche la qualité du signal et l’état de la connexion réseau.

## Composants matériels
- **ESP32 TTGO T-Call SIM800L** : Microcontrôleur avec module GSM SIM800L intégré.
- **Écran OLED 128x64** : Pour afficher les statuts (interface I2C, adresse 0x3C).
- **Boutons** : Pour initier et terminer les appels.
- **Microphone et haut-parleur** : Réutilisés à partir d’écouteurs, soudés avec des fils conducteurs pour l’entrée/sortie audio.

## Dépendances logicielles
- **Arduino IDE** : Pour programmer l’ESP32.
- **Bibliothèques** :
  - `TinyGsmClient` (pour la communication avec le SIM800L)
  - `Adafruit_GFX` et `Adafruit_SSD1306` (pour l’écran OLED)
  - `Wire` (pour la communication I2C)
  - `FreeSans9pt7b` (police personnalisée pour l’écran OLED)

## Instructions de configuration
1. **Configuration matérielle** :
   - Branchez l’écran OLED aux broches I2C (SDA=21, SCL=22).
   - Connectez le bouton vert à la broche 14 et le bouton rouge à la broche 12 (avec résistances pull-up).
   - Soudez le microphone et le haut-parleur aux broches audio du SIM800L pour la communication vocale.
   - Insérez une carte SIM avec un code PIN optionnel (défini dans la variable `simPIN` si nécessaire).

2. **Configuration logicielle** :
   - Installez l’Arduino IDE et les bibliothèques requises (`TinyGsmClient`, `Adafruit_GFX`, `Adafruit_SSD1306`).
   - Mettez à jour le numéro de téléphone dans le code (`PHONE_NUMBER` pour le numéro complet avec indicatif, `PHONE_NUMBER2` pour le numéro local).
   - Téléversez le croquis `appel_gsm_ecocar.ino` sur l’ESP32 via l’Arduino IDE.

3. **Configuration** :
   - Assurez-vous que la carte SIM est active et a une couverture réseau.
   - Modifiez la variable `simPIN` si votre carte SIM nécessite un code PIN.
   - Vérifiez la configuration audio (microphone et haut-parleur) pour une communication claire.

## Fonctionnement
1. **Initialisation** :
   - Le système initialise le module SIM800L, l’écran OLED et la gestion de l’alimentation.
   - Il vérifie l’état de la carte SIM et la connexion réseau, affichant « Connecté » ou « Pas de connexion » sur l’écran OLED.

2. **Actions du pilote** :
   - **Bouton vert** : Appuyez pour initier un appel vers le numéro d’équipe prédéfini. L’écran OLED affiche « Appel lancé ».
   - **Bouton rouge** : Appuyez pour terminer l’appel, affichant « Appel terminé » sur l’écran OLED.

3. **Appels entrants** :
   - Si l’appel provient du numéro d’équipe prédéfini, le système répond automatiquement, configure le haut-parleur et règle le volume au maximum, affichant « Haut-parleur activé » et « Appel commencé ».
   - Si l’appel provient d’un numéro inconnu, il est automatiquement rejeté, et l’écran OLED affiche « Appel étranger... Raccrochage... ».

4. **Retour d’état** :
   - L’écran OLED affiche des informations en temps réel, telles que la qualité du signal, l’état audio et la progression de l’appel.
   - Après la fin d’un appel, le système revient à « Système prêt » après un délai de 3 secondes.

## Aperçu du code
Le fichier `appel_gsm_ecocar.ino` contient le croquis Arduino complet pour le système. Les fonctions clés incluent :
- `setup()` : Initialise le modem, l’écran OLED, les boutons et la gestion de l’alimentation.
- `makePhoneCall()` : Gère les appels sortants, y compris la vérification de la qualité du signal et la configuration audio.
- `hangUpCall()` : Termine les appels actifs avec une confirmation affichée.
- `checkATResponse()` : Analyse les réponses du modem pour la gestion des erreurs.
- `loop()` : Surveille les appuis sur les boutons et les appels entrants, mettant à jour l’écran OLED en conséquence.

## Utilisation
- **Pilote** : Appuyez sur le bouton vert pour appeler l’équipe et sur le bouton rouge pour terminer l’appel. Suivez les statuts sur l’écran OLED.
- **Équipe** : Appelez le numéro du SIM800L pour communiquer avec le pilote ; le système répond automatiquement si le numéro correspond à celui prédéfini.
- **Dépannage** :
  - Vérifiez la carte SIM et le signal réseau en cas d’échec des appels.
  - Assurez-vous que le microphone et le haut-parleur sont correctement connectés pour l’audio.
  - Surveillez l’écran OLED pour les messages d’erreur (par exemple, « Réseau bas !! »).

## Améliorations futures
- Ajouter une fonctionnalité SMS pour une communication textuelle.
- Améliorer la qualité audio avec une réduction de bruit ou un meilleur matériel.
- Ajouter la prise en charge de plusieurs numéros d’équipe prédéfinis.

## Contribution
Les contributions sont les bienvenues ! Veuillez soumettre des issues ou des pull requests pour des corrections de bugs, des ajouts de fonctionnalités ou des améliorations de la documentation.

## Remerciements
- Équipe EcoCar pour l’inspiration et le soutien.
- Communautés Arduino et TinyGSM pour leurs bibliothèques open-source.
