#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <fstream>


using namespace std;

using matrix = vector<vector<size_t> > ; // Changement de nom de vector<vector<int> >

void reproduction_clonale(matrix const& parents, matrix& offspring, mt19937_64& r)
{
    uniform_int_distribution<size_t> u_size_t(0, parents.size()-1);

    for (size_t i = 0; i < offspring.size(); i = i + 1)
    {
        size_t i_parents = u_size_t(r) ;
        offspring[i] = parents[i_parents] ;
    }
}

void reproduction_sexuee(matrix const& parents, matrix& offspring, mt19937_64& r)
{
    uniform_int_distribution<size_t> u_size_t(0, parents.size()-1);
    uniform_real_distribution<double> u ;

    for (size_t i = 0; i < offspring.size(); i = i + 1)
    {
        size_t i_pere = u_size_t(r) ;
        size_t i_mere = u_size_t(r) ;

        for (size_t j = 0; j < (offspring[i].size()/2); j = j + 1)
        {
            offspring[i][j] = parents[i_pere][j] ;
        }

        for (size_t j = (offspring[i].size()/2); j < offspring[i].size(); j = j + 1)
        {
            offspring[i][j] = parents[i_mere][j] ;
        }
    }
}

void reproduction_sexuee_2(matrix const& parents, matrix& offspring, mt19937_64& r)
{
    uniform_int_distribution<size_t> u_size_t(0, parents.size()-1);
    uniform_real_distribution<double> u ;

    for (size_t i = 0; i < offspring.size(); i = i + 1)
    {
        size_t i_pere = u_size_t(r) ;
        size_t i_mere = u_size_t(r) ;

        for (size_t j = 0; j < offspring[i].size(); j = j + 1)
        {
            if(u(r) < 0.5) {
                offspring[i][j] = parents[i_pere][j] ;
            }
            else {
                offspring[i][j] = parents[i_mere][j] ;
            }

        }
    }
}

void reproduction_melange(matrix const& parents, matrix& offspring, mt19937_64& r)
{
    uniform_int_distribution<size_t> u_size_t(0, parents.size()-1);

    for (size_t i = 0; i < offspring.size(); i = i + 1)
    {
        for (size_t j = 0; j < offspring[i].size(); j = j + 1)
        {
            size_t i_parents = u_size_t(r) ;
            offspring[i][j] = parents[i_parents][j] ;
        }
    }
}

class reproduction_fonction
{
private :
    mt19937_64& _r ;
    function<void(matrix const&, matrix&, mt19937_64& )> _f;

public :
    reproduction_fonction(char type_reproduction, mt19937_64& r) :
        _r(r)
    {
        if (type_reproduction == '1') _f = reproduction_clonale ;
        else if (type_reproduction == '2') _f = reproduction_sexuee ;
        else if (type_reproduction == '3') _f = reproduction_sexuee_2 ;
        else if (type_reproduction == '4') _f = reproduction_melange ;
    }

    void operator()(matrix const& parents, matrix& offspring)
    {
        _f(parents, offspring, _r) ;
    }

};

void mutation(matrix& offspring, double mu, mt19937_64& r)
{
    uniform_real_distribution<double> u ;
    uniform_int_distribution<size_t> u_size_t ;

    for (size_t i = 0; i < offspring.size(); i = i + 1)
    {
        for (size_t j = 0; j < offspring[i].size(); j = j + 1)
        {
            if (u(r) < mu) offspring[i][j] = u_size_t(r) ;
        }
    }
}


int main(int argc, char *argv[])
{
    try
    {

        string nom_simulation = "simulation" ;
        if (argc > 1) nom_simulation = argv[1] ;

        char type_reproduction = '1' ;
        if (argc > 2) {
            type_reproduction = (char) *argv[2] ;
        }

        size_t Ne_0_min = 100 ;
        size_t Ne_0_max = 1000 ;

        size_t Ne_1_min = 100 ;
        size_t Ne_1_max = 1000 ;

        size_t t_0_min = 1 ;
        size_t t_0_max = 1000 ;

        double mu_max = 0.1 ;

        size_t nombre_cognats = 185 ;
        size_t nombre_echantillon = 80 ;


        mt19937_64 r{random_device()()} ;
        uniform_real_distribution<double> u ;
        uniform_int_distribution<size_t> uniform_Ne_0(Ne_0_min, Ne_0_max) ;
        uniform_int_distribution<size_t> uniform_Ne_1(Ne_1_min, Ne_1_max) ;
        uniform_int_distribution<size_t> uniform_t_0(t_0_min, t_0_max) ;
        uniform_int_distribution<size_t> u_size_t ;

        size_t Ne_0 = uniform_Ne_0(r) ;
        size_t Ne_1 = uniform_Ne_1(r) ;
        size_t t_0 = uniform_t_0(r) ;
        double mu = u(r)*mu_max ;
/*
        vector<double> distribution ;
        double alpha(1) ;
        double beta(10) ;

        gamma_distribution<double> gammaAlpha(alpha, 1.0) ;
        gamma_distribution<double> gammaBeta((double) beta*(1 - mu)/mu, 1.0) ;

        for (int i = 0; i < nombre_cognats; i++)
        {
            distribution.push_back( (double) gammaAlpha(r) / (gammaAlpha(r) + gammaBeta(r)) ) ;
        }
*/
        //string fichierEq = nom_simulation + ".eq" ;
        string nom_fichier_fin  = nom_simulation + ".ech" ;
        string nom_fichier_param  = nom_simulation + ".par" ;
        remove((char*)nom_fichier_fin.c_str()) ;
        ofstream outputFin((char*)nom_fichier_fin.c_str(), ios::app) ;
        ofstream outputParam((char*)nom_fichier_param.c_str(), ios::app) ;

        outputParam << "N0,N1,t,m" << endl << Ne_0 << "," << Ne_1 << "," << t_0 << "," << mu ;
        /*
        for (int i = 0; i < nombre_cognats; i++)
        {
            outputParam << distribution[i] << " " ;
        }
*/
        matrix population ;
        matrix offspring ;

        size_t Ne = Ne_0 ;
        vector<size_t> clone ;

        for (size_t i = 0 ; i < nombre_cognats ; i = i + 1) clone.push_back(u_size_t(r)) ;
        for (size_t i = 0 ; i < Ne ; i = i + 1) population.push_back(clone) ;

        offspring = population ;
        reproduction_fonction reproduction((char) type_reproduction, r );

        for (size_t t = 0 ; t <= Ne*5 ; t = t + 1)
        {
            reproduction(population, offspring) ;
            mutation(offspring, mu, r) ;

            swap(offspring, population) ;
            //for (size_t i = 0 ; i < Ne ; i = i + 1) { for (size_t j = 0 ; j < nombre_cognats ; j = j + 1) {output << population[i][j] << " " ; } output << endl ; } output << endl ;
            //cout << t << endl ;
            //cout << t << endl ;
        }

        //for (size_t i = 0 ; i < min(nombre_echantillon, Ne) ; i = i + 1) { for (size_t j = 0 ; j < nombre_cognats ; j = j + 1) {outputEq << population[i][j] << " " ; } outputEq << endl ; } outputEq << endl ;

        Ne = Ne_1 ; // Perturbation dans la taille des populations
        offspring.erase(offspring.begin(), offspring.end()) ;
        for (size_t i = 0 ; i < Ne ; i = i + 1) offspring.push_back(clone) ;

        reproduction(population, offspring) ;
        mutation(offspring, mu, r) ;

        population = offspring ;


        for (size_t t = 0 ; t <= t_0 ; t = t + 1)
        {
            reproduction(population, offspring) ;
            mutation(offspring, mu, r) ;

            swap(offspring, population) ;

            //for (size_t i = 0 ; i < Ne ; i = i + 1) { for (size_t j = 0 ; j < nombre_cognats ; j = j + 1) {output << population[i][j] << " " ; } output << endl ; } output << endl ;
        }



        for (size_t i = 0 ; i < min(nombre_echantillon, Ne) ; i = i + 1) { for (size_t j = 0 ; j < nombre_cognats ; j = j + 1) {outputFin << population[i][j] << "," ; } outputFin << endl ; } outputFin << endl ;


    }
    catch( exception& e )
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;

}
