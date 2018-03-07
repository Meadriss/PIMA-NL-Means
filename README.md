# PIMA-NL-Means
projet de M1 sur le filtrage d'image de manière non local

# Compilation

Le projet seras dévelloper en C++ avec l'utilisation de la librairie CImg (distribué sous la licence CeCILL) pour modifier et afficher les images.
http://cimg.eu/

linux : g++ nlmeans.cpp -o nlmeans -O2 -lm -L/usr/X11R6/lib -lpthread -lX11 -std=c++11

pour lancer le projet : ./nlmeans -i image -s patchSize (-n noise si l'image n'est pas bruité && -h pour le sigma du bruit)

valeur -n : 0 -> gaussian, 1 -> uniforme, 2 -> Salt & Pepper, 3 -> Poisson, 4 -> Rician

# Bibliographie

Antoni Buades, Bartomeu Coll, Jean-Michel Morel. A non-local algorithm for image denoising. Computer Vision and Pattern Recognition, 2005.
https://perso.telecom-paristech.fr/bloch/P6Image/Projets/NLMeans.pdf

Connelly Barnes, Eli Shechtaman, Adam Finkelstein, Dan B Goldman. PatchMatch: A Randomized Correspondence Algorithm for Structural Image Editing. ACM Transactions on Graphics (Proc. SIGGRAPH) 28(3), August 2009.
http://gfx.cs.princeton.edu/pubs/Barnes_2009_PAR/patchmatch.pdf

# Resultat

---- NLMEANS ----

commande : ./nlmeans -i Picture/barbara.png -h 10 -n 0 -s 7

    Original                                 Bruité/ZeroPadding                               Resultat
![ScreenShot](/Resultat/Resultat_NL_MEANS.png)
