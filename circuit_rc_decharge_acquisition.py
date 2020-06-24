# -*- coding: utf-8 -*-
from serial import *
from numpy import *
import struct

"""Données pour la connexion série
"""
nom_port = "/dev/ttyACM0" # A modifier suivant le branchement de l'arduino
vitesse_baud = 115220  # Doit être le même que dans le programme Arduino
read_timeout = 1
write_timeout = 1

"""Récupération des réglages

On demande le nom du fichier pour enregistrer les données.
"""
# filename = input("Entrez le nom du fichier (sans extension - attention si le fichier existe, son contenu sera supprimé) : ") + ".dat"
filename = "circuit_rc_decharge.dat"

# Entrée de la durée d'acquisition
# duree = int(input("Entrez la durée d'acquisition en millisecondes (MAX : 1000) : ").strip())
duree = 100
# Ouverture de la communication avec l'Arduino
temps = []
tension = []
with open(filename, 'w') as file:
    with Serial(port=nom_port, baudrate=vitesse_baud, timeout=read_timeout, writeTimeout=write_timeout) as port_serie:
        if port_serie.isOpen():
            time.sleep(2)  # Un temps de latence est nécessaire au démarrage de la communication série
            # Les deux lignes suivantes servent à vider le buffer au cas où
            toread = port_serie.inWaiting()
            port_serie.read(toread)
            port_serie.write("P".encode('ascii'))  # Pour prévenir Arduino que c'est Python qui parle
            port_serie.write(struct.pack('>H', duree))  # On envoie à Arduino la durée d'acquisition
            time.sleep(.1)
            tic = time.time()  # Fait office de timeout dans le programme qui attend les mesures.
            toread = port_serie.inWaiting()  # Nombre de bytes dans le buffer
            while(toread < 1 and time.time() - tic < 2):  # On attend le début de l'envoi
                toread = port_serie.inWaiting()
            print("Début de la mesure")
            tic = time.time()  # Fait office de timeout dans le programme qui attend les mesures.
            acquisition = True
            n = 0
            while(acquisition and time.time() - tic < duree / 1000 * 2):  # Tant qu'on ne lit pas un "E" sur le buffer on enregistre la mesure
                toread = port_serie.inWaiting()
                while(toread < struct.calcsize('HH') and time.time() - tic < duree / 1000 * 2):
                    toread = port_serie.inWaiting()
                nbread = toread // struct.calcsize('HH')
                if(nbread > 0):
                    mesure = port_serie.read(nbread * struct.calcsize('HH'))
                    mesure = struct.unpack(nbread * 'HH', mesure)
                    mesure = reshape(mesure, (nbread, 2))
                    temps += list(mesure[:, 0])
                    tension += list(mesure[:, 1])
        mesures = transpose([temps, tension])
        savetxt(file, mesures, fmt="%i", delimiter=',', header="t(microsec), uc(echelle -(0-5)V vers (0-1023)B)")
