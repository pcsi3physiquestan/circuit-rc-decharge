# -*- coding: utf-8 -*-
from pylab import *
from scipy import stats
from scipy.optimize import curve_fit

"""
Ce programme va traiter les données obtenues par étude de la décharge d'un condensateur.
Le fichier d'enregistrement contient une premier ligne d'en-tête puis les mesures de temps caractéristiques.
On va dans ce programme étudier la décharge et remonter à la valeur de C.
"""

""" Importation des données
On enregistre les données de mesures dans un tableau :
- temps : temps caractéristiques mesurées
"""

filename = "circuit_rc_auto.dat"
with open(filename, 'r') as file:
    temps = np.loadtxt(file, delimiter=",", skiprows=1)
    n_mesures = temps.size

"""
On passe en millisecondes le temps pour plus de lisibilité
"""
temps = temps / 1000

""" Tracé de l'histogramme des valeurs de tau"""
f = figure(1)  # On crée une figure
ax1 = f.add_subplot(1, 1, 1)  # Création des axes du graphiques
ax1.hist(temps)  # Création de la courbe

ax1.set_xlabel(r"$t(ms)$")  # Légende des axes

f.savefig('tau_decharge_hist.png')

"""Les temps sont quasiment tous identiques, ce qui est plutôt une bonne nouvelle. On ne note pas de mesure clairement aberrante.
On va pouvoir estimer le temps caractéristique moyen puis la capacitance.

L'incertitude sur tau est principalement dû aux décalage possible dans la mesure interne à Arduino.
On peut la majorer par le temps d'échantillonnage qui est d'environ 0.4 ms.
"""

tau = mean(temps)
utauA = std(temps, ddof = 1)/ sqrt(n_mesures)  # On calcule l'incertitude de type A qu'on va composer avec l'incertitude évoquée précédemment.
utau = sqrt((0.4)**2 + utauA**2)
print ("tau = {:.1f} +/- {:.1f} ms".format(tau, utau))

R = 1e5
uR = 0.1 / 100 * R
C = tau / R * 1e6  # On passe C en nanoFarad pour plus de lisibilité
uC = C *sqrt((uR / R)**2 + (utau/tau)**2)
print ("C = {:.0f} +/- {:.0f} nF".format(C, uC))
print("Valeur constructeur : 100 nF")