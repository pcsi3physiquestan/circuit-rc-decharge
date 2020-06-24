# -*- coding: utf-8 -*-
from pylab import *
from scipy import stats
from scipy.optimize import curve_fit

"""
Ce programme va traiter les données obtenues par étude de la décharge d'un condensateur.
Le fichier d'enregistrement contient une premier ligne d'en-tête puis les couples de mesures (temps, tension aux bornes de u_C).
On va dans ce programme étudier la décharge et remonter à la valeur de C.
"""

""" Importation des données
On enregistre les données de mesures dans plusieurs tableaux :
- temps : temps des mesures
- tensions : valeurs en binaire des tensions mesurées
"""

filename = "circuit_rc_decharge_2.dat"
with open(filename, 'r') as file:
    datas = np.loadtxt(file, delimiter=",", skiprows=1)
    temps = np.array(datas[:, 0])
    n_mesures = temps.size
    tensions = np.array(datas[:, 1])

"""
On passe en millisecondes le temps pour plus de lisibilité
"""
temps = temps / 1000

"""
Les tensions sont à l'heure actuelle en binaire (ici un nombre entre 0 et 1023).
On commence par les convertir en des valeurs de tensions entre 0 et 5V.
"""
B_MAX = 1023
U_MAX = 5
B_MIN = 0
U_MIN = 0
tensions = (tensions - B_MIN) / (B_MAX - B_MIN) * (U_MAX - U_MIN) + U_MIN

""" Tracé de la courbe u(t)"""
f = figure(1)  # On crée une figure
ax1 = f.add_subplot(1, 1, 1)  # Création des axes du graphiques
ax1.plot(temps, tensions, label=r"$u_C(t)$", linestyle=':', linewidth=1, color="black")  # Création de la courbe

ax1.set_ylabel(r"$u_C(V)$")  # Légende des axes
ax1.set_xlabel(r"$t(ms)$")  # Légende des axes
ax1.legend()  # Fait apparaître la légende à droite du graphique

"""
L'allure a l'air d'être cohérent avec une loi exponentielle. On va réaliser un ajustement pour déterminer la constante de temps.
"""
"""Définition du modèle

On commence par définir une fonction correspondant au modèle d'étude.
On prend comme modèle uC(t) = A e^(-(t-t0)/tau).
Le t0 permet de prendre en compte un possible retard au démarrage de l'acquisition (ou pas).
"""


def modele_exp(t, tau, A, t0, B):
    y = A * np.exp(-(t - t0) / tau) + B
    return y

def modele_exp_2(t, tau, A):
    y = A * np.exp(-(t) / tau)
    return y

# params, corel = curve_fit(modele_exp, temps, tensions, p0 = [1., 1., 1., 0.])  # Cette fonction réalise l'ajustement et renvoie dans params les paramètres optimaux.
params, corel = curve_fit(modele_exp_2, temps, tensions, p0 = [1., 1.])  # Cette fonction réalise l'ajustement et renvoie dans params les paramètres optimaux.



lntensions_fit = modele_exp_2(temps, *params) # On applique les paramètres d'ajustment aux distance pour pouvoir tracer le modèle et les résultats.
ax1.plot(temps, lntensions_fit, label="Courbe de tendance", linestyle='-', linewidth=1, color="red")  # Création de la courbe

ax1.legend()  # Fait apparaître la légende à droite du graphique

ax1.text( 0, 0, r"$\tau = {:.1f} ms$".format(params[0]))
C = params[0] * 10
ax1.text( 0, 0.2, r"$C = {:.1f} \mu F$".format(C))
print(params)
show()  # Pour afficher le graphique
