#include "Entete.h"
#pragma comment (lib,"TabouDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (TabouDLL.dll et TabouDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur. 
//Indiquer les arguments du programme dans les proprietes du projet - debogage - arguments.
//Sinon, utiliser le repertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TAlgo & unAlgo);

//DESCRIPTION:	Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme (TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'ecran pour validation
extern "C" _declspec(dllimport) void AfficherSolution(const TSolution uneSolution, TProblem unProb, std::string Titre);

//DESCRIPTION:	Evaluation de la fonction objectif d'une solution et MAJ du compteur d'evaluation. La fonction objectif represente la distance totale de la tournee incluant le retour un point initial.
//				NB: L'evaluation d'une solution inclue la validation de la presence de chacune des villes une et une seule fois
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution & uneSol, TProblem unProb, TAlgo & unAlgo);

//DESCRIPTION:	Creation d'une sequence (permutation) aleatoire de villes et appel a l'evaluation de la fonction objectif.
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution & uneSolution, TProblem unProb, TAlgo & unAlgo);

//DESCRIPTION: Affichage a l'ecran de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultats (const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier(en append) de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultatsFichier (const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
extern "C" _declspec(dllimport) void	LibererMemoireFinPgm	(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);


bool SolEstAdmissible(const vector<TSolution>& listeTaboux, const TSolution& sol, long critere_aspiration);

//DESCRIPTION: Deplacement de 1-2-3 villes selectionnees aleatoirement.												
void OrOPT(TSolution& Voisin, TProblem unProb, TAlgo& unAlgo);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TSolution Courante;		//Solution active au cours des iterations
	TSolution Next;			//Solution voisine retenue a une iteration
	TSolution Best;			//Meilleure solution depuis le debut de l'algorithme	//Non utilisee pour le moment 
	TProblem LeProb;		//Definition de l'instance de probleme
	TAlgo LAlgo;			//Definition des parametres de l'agorithme
	string NomFichier;
		
	//**Lecture des parametres
	NomFichier.assign(Param[1]);
	LAlgo.TailleVoisinage = atoi(Param[2]);
	LAlgo.LngListeTabous = atoi(Param[3]);
	LAlgo.NB_EVAL_MAX = atoi(Param[4]);

	srand((unsigned) time(NULL));		//**Precise un germe pour le generateur aleatoire
	
	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);
	
	//**Creation de la solution initiale 
	CreerSolutionAleatoire(Courante, LeProb, LAlgo);
	AfficherSolution(Courante, LeProb, "SOLUTION INITIALE: ");
	//**Enregistrement qualite solution de depart
	LAlgo.FctObjSolDepart = Courante.FctObj;
	
	do
	{
		Next = GetSolutionVoisine(Courante, LeProb, LAlgo);
		//AfficherSolution(Courante, LeProb, "COURANTE: ");
		//AfficherSolution(Next, LeProb, "NEXT: ");
		if (Next.FctObj <= Courante.FctObj)	//**deplacement amelioration ou egalite
		{
			Courante = Next;
			cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Courante.FctObj << endl;
			//AfficherSolution(Courante, LeProb, "NOUVELLE COURANTE: ");
		}
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj!=0); //Critere d'arret
	
	AfficherResultats(Courante, LeProb, LAlgo);
	AfficherResultatsFichier(Courante, LeProb, LAlgo,"Resultats.txt");
	
	LibererMemoireFinPgm(Courante, Next, Best, LeProb);
	
	//system("PAUSE");

	return 0;
}

//DESCRIPTION: Creation d'une solution voisine a partir de la solution courante (uneSol) qui ne doit pas etre modifiee.
//Dans cette fonction, appel de la fonction AppliquerVoisinage() pour obtenir une solution voisine selon un TYPE DE VOISINAGE selectionne + Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//Ainsi, si la RÈGLE DE PIVOT necessite l'etude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage()" sera appelee plusieurs fois.
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//Type (structure) de voisinage : 	OrOPT - Deplacement de 1, 2 ou 3 villes selectionnees aleatoirement 
	//Parcours dans le voisinage : 		Aleatoire: taille du bloc aleatoire entre [1,3], positions de selection et d'insertion aleatoires 
	//Regle de pivot : 					k-ImproveBEST (k étant donné en paramètre pour l'exécution du pgm)

	std::vector<TSolution> listeTaboux(unAlgo.LngListeTabous, TSolution());

	//k-Improve-Best
	TSolution unVoisin, unGagnant;
	int i;
	
	unGagnant = AppliquerVoisinage(uneSol, unProb, unAlgo); //Premier voisin
	static long critere_aspiration = unGagnant.FctObj;

	for (i = 1; i < unAlgo.TailleVoisinage; i++) //Permet de generer plusieurs solutions voisines
	{
		unVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);

		// Si le voisin est admissible 
		if(SolEstAdmissible(listeTaboux, unVoisin, critere_aspiration))
		{
			if (unVoisin.FctObj < unGagnant.FctObj) //Conservation du meilleur
				unGagnant = unVoisin;
			else //Choix aléatoire si unVoisin et un Gagnant sont de même qualité
				if (unVoisin.FctObj == unGagnant.FctObj)
				{
					if (rand() % 2 == 0)
						unGagnant = unVoisin;
				}
		}
	}
	return (unGagnant);
}

//DESCRIPTION: Fonction appliquant le type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB: La solution courante (uneSol) ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	//Type (structure) de voisinage : 	OrOPT - Deplacement de 1, 2 ou 3 villes selectionnees aleatoirement	
	TSolution Copie;

	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	Copie = uneSol;

	//Transformation de la solution Copie selon le type (structure) de voisinage selectionne : OrOPT
	OrOPT(Copie, unProb, unAlgo);																										

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}


bool SolEstAdmissible(const vector<TSolution>& listeTaboux, const TSolution& sol, long critere_aspiration)
{
	// Si la solution n'est pas présente dans la liste des taboux ou qu'elle vérifie le critère d'aspiration
	if (std::find(listeTaboux.begin(), listeTaboux.end(), sol) == listeTaboux.end() 
		|| sol.FctObj < critere_aspiration)
	{
		// La solution est admissible
		return true;
	}

	return false;
}

//DESCRIPTION: Deplacement de 1-2-3 villes selectionnees aleatoirement
void OrOPT(TSolution& Voisin, TProblem unProb, TAlgo& unAlgo)
{
	#define MAX 3	//Taille maximale pour le bloc de villes a deplacer
	int PosSelec;	//Variable pour la position du bloc a selectionner
	int PosInsert;	//Variable pour la position d'insertion
	int NbVillesDispo, TailleBloc;
	vector<int> Bloc;

	//Tirage aleatoire de la position de selection du bloc
	PosSelec = rand() % unProb.NbVilles;

	//Tirage aleatoire de la taille du bloc (1, 2 ou 3 villes) NB: peut etre limite par nombre de villes disponibles (jusqu'a la fin de la solution) a partir de la position de selection du bloc
	TailleBloc = (rand() % MAX) + 1;
	NbVillesDispo = Voisin.Seq.size() - PosSelec;
	TailleBloc = min(TailleBloc, NbVillesDispo);

	//Extraction du bloc a deplacer
	Bloc = { Voisin.Seq.begin() + PosSelec, Voisin.Seq.begin() + PosSelec + TailleBloc };
	Voisin.Seq.erase(Voisin.Seq.begin() + PosSelec, Voisin.Seq.begin() + PosSelec + TailleBloc);

	//Tirage aleatoire de la position d'insertion (en fonction de la taille restante du vecteur)
	PosInsert = rand() % Voisin.Seq.size();

	//Insertion du bloc a la position d'insertion
	Voisin.Seq.insert(Voisin.Seq.begin() + PosInsert, Bloc.begin(), Bloc.end());
}