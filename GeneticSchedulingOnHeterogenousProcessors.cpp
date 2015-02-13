
/* SOME MODIFICATIONS STILL REQUIRED */

#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<cstdio>
#include<cstdlib>
#include<sstream>

using namespace std;

#define pb push_back
#define F first
#define S second
#define mk make_pair
#define vi vector<int>
#define vd vector<double>

/*  Tasks start from 0 to n-1 */
/* Processors start from 0 to n-1 */

/* Class equivalent of c++ pair */
class node{
    public:
    int idx;
    int val;
    node(int i,int j){
        idx = i;
        val = j;
    }
};

/* Compare function to reverse sort node using val attribute */
bool cmp(node a,node b){
    return a.val > b.val;
}

/* Class for a chromosome of population */
class chromosome{
    public:
    double fitness;
    vector<int> R;
    vector<int> A;
};

bool cmp2(chromosome a,chromosome b){
    return a.fitness > b.fitness;
}

/* Class to take task DAG input from file*/
class IO{

	enum info {Hyperperiod,No_of_Tasks,No_of_Processors,t_id,t_rtime,t_period,t_dline,t_nst,No_of_pairs};
	vector<int> valueOf;
	map<int,vector<int> > st;
	map<int,vector<pair<int,int> > > Link;
	vector<vector<double> > E_P;
	vector<vector<double> > E_PP;
	vector<vector<vector<double> > > Pr_Levl;
	vector<vector<int> > * dag;
    vector<vector<int> > * pred;

	public:

	IO(){

		while(true){

			string str;
			getline(cin,str);

			if( str.find('#') != string::npos ){
				continue;
			}

			string buf;
			stringstream ss(str);
			vector<string> tokens;

			while(ss >> buf){
				tokens.push_back(buf);
			}

			valueOf.push_back(atoi(tokens[1].c_str()));

			if( str.find("t_nst") != string::npos ){
				break;
			}
		}


		for(int i = 0 ; i < valueOf[t_nst] ; i++ ){
			string temp;
			cin>>temp;
			for(int j = 0 ; j < valueOf[No_of_Processors] ; j++ ){
				int val;cin>>val;
				st[i].push_back(val);
			}
		}

		while(true){
			string temp;
			int to , from , cost;
			cin>>temp>>from;
			if(from == -1 ) break;
			cin>>to>>cost;
			Link[from].push_back(make_pair(to,cost));
		}

		dag = new vector<vector<int> >(nTasks(),vector<int>(nTasks(),0));

    	for(int i=0;i<nTasks();i++){
			for(int j=0;j<adjecency(i);j++){
				(*dag)[i][link(i,j).first] = link(i,j).second;
			}
    	}

        pred = new vector<vector<int> >(nTasks());

        for(int i=0;i<nTasks();i++){
            for(int j=0;j<nTasks();j++){
                if(  (*dag)[i][j]) {
                    (*pred)[j].push_back(i);
                }
            }
        }
	}

	void prn_st(){
		for(int i=0;i<st.size();i++){
			for(int j=0;j<st[i].size();j++){
				cout<<st[i][j]<<" ";
			}
			cout<<endl;
		}
	}

	void prn_dag(){
		for(int i=0;i<(*dag).size();i++){
			for(int j=0;j<(*dag)[i].size();j++){
				cout<<(*dag)[i][j]<<" ";
			}
			cout<<endl;
		}
	}

	void prn_pred(){
		for(int i=0;i<(*pred).size();i++){
		cout<<i<<" -- ";
			for(int j=0;j<(*pred)[i].size();j++){
				cout<<(*pred)[i][j]<<" ";
			}
			cout<<endl;
		}
	}

	int nTasks(){
		return valueOf[t_nst];
	}

	int nProcs(){
		return valueOf[No_of_Processors];
	}

	int adjecency(int i){
		return Link[i].size();
	}

	pair<int,int> link(int i,int j){
		return Link[i][j];
	}

    int exec_time(int i,int j){
        return st[i][j];
    }

    int pred_size(int idx){
        return (*pred)[idx].size();
    }

    int pred_value(int i,int j){
        return (*pred)[i][j];
    }

    int comm_time(int i,int j){
        return (*dag)[i][j] ;
    }

};

/* Class for Genetic Scheduling */
class geneticScheduling : public IO
{

    int maxIterations;
    int maxPopulation ;
    double mutationProbability;
    double cossoverProbability;
    vector<chromosome> population;

public:

    /* Return the number of chromosome at any instant of time */
    int populationSize(){
        return population.size();
    }

    int maxPop(){
        return maxPopulation;
    }

    /* Function to find a given value in an array */
    int Find( vector<int>&a, int l,int r,int val){
        for( int i = l ; i <= r ; i++ ){
            if( a[i] == val ) return i;
        }
        return -1;
    }

    /* Constructor for the class */
    geneticScheduling( int init_maxIterations ,int init_maxPopulation, double init_mutationProbability , double init_crossoverProbability ){
        maxIterations = init_maxIterations ;
        mutationProbability = init_mutationProbability;
        cossoverProbability = init_crossoverProbability;
        maxPopulation = init_maxPopulation;
    }

    /* Recursively compute the priority of each task */
    int priority(int Task,vector<int>&re,vector<int>&p){

        if( p[Task] > 0 ) return p[Task];

        for( int i = 0 ; i < nTasks() ; i++ ){
            if( comm_time(Task,i) ){
                p[Task] = max( p[Task] , priority(i,re,p) + re[Task]);
            }
        }
        return p[Task];
    }

    /* Function to convert task graph to a chromosome of the population (no) */
    chromosome dagToChromosome(){

		int entryTask;
		for( int i = 0 ; i < nTasks() ; i++ ){
			if( pred_size(i) == 0 ){
				entryTask = i;
				break;
			}
		}

		vector<int> re(nTasks(),0) , p(nTasks(),0);

		for(int i=0;i<re.size();i++){
			re[i] = rand()%(nTasks()) + 1;
		}

        for( int i = 0 ; i < nTasks() ; i++ ){
            if( comm_time(entryTask,i) ){
                p[entryTask] = max( p[entryTask] , priority(i,re,p)+re[entryTask] );
            }
        }

        vector<node> temp;

        for(int i=0;i<p.size();i++){
            temp.push_back(node(i,p[i]));
        }

        sort(temp.begin(),temp.end(),cmp);

        chromosome c;
        for(int i=0;i<temp.size();i++){
            c.R.push_back(temp[i].idx);
            c.A.push_back(rand()%nProcs());
        }

        computeFitnessValue(c);
        return c;
    }

    /* Returns the processor on which task 'task_id' is scheduled */
    int proc_idx(int task_id,chromosome&ch){
        int idx = Find(ch.R,0,ch.R.size()-1,task_id);
        return ch.A[idx];
    }

    /* Function to calculate the fitness value of the chromosome (done) */
    void computeFitnessValue( chromosome & ch ){

        int makespan = 0;
        vector<int> st(nTasks(),0) , ft(nTasks(),0) , Av(nProcs(),0) ;

        ft[0] = exec_time(0,proc_idx(0,ch));
        Av[proc_idx(0,ch)] = ft[0];

        for( int i = 1 ; i < nTasks() ; i++ ){

        	int a = proc_idx(i,ch);

        	for(int k = 0 ; k<pred_size(i) ;k++){
        		int p = pred_value(i,k);
        		int b = proc_idx(p,ch);
        		st[i] = max( st[i] , max( (a!=b)*comm_time(p,i) + ft[p] , Av[a] ) );
        	}

        	ft[i] = st[i] + exec_time(i,a);
        	Av[a] = max(ft[i],Av[a]);
        }

        for(int i=0;i<nTasks();i++){
        	makespan = max( makespan , ft[i] ) ;
        }

		if(makespan == 0 ) cout<<"makespan = 0 "<<endl;
        else ch.fitness = 1.0/double(makespan);

    }

    int computeFitnessValue( vector<chromosome> & ch ){

        for( int i = 0 ; i < ch.size() ; i++ ){
            computeFitnessValue(ch[i]) ;
        }
    }

    /* Function for mutation in chromosome (done) */
    void mutate( chromosome & ch ){

        for( int i = 0 ; i < nTasks() ; i++ ){

            double val = ( 1.0 * rand() ) / RAND_MAX;

            if( val <= mutationProbability ){

                int temp = ch.A[i];
                while(  ( ch.A[i] = rand() % nProcs() ) == temp );

            }
        }
    }

    void mutate( vector<chromosome> & ch ){
        for(int i=0;i<ch.size();i++){
            mutate(ch[i]);
        }
    }

    /* Function to select good chromosomes (done) */
    void selectParents( vector<chromosome> & parents ){
        int p1 = rand()%population.size(),p2;
        while( p1 == ( p2 = rand()%population.size() ) ) ;
        parents.push_back(population[p1]);
        parents.push_back(population[p2]);
    }

    /* Function to combine the two parent chromosomes (done) */
    void produceChildren( vector<chromosome> & parents , vector<chromosome> & children ){

        int pos = rand()%nTasks();
        chromosome c1,c2;

        for(int i=0;i<=pos;i++){
            c1.R.push_back(parents[1].R[i]);
        }

        for(int i=0;i<parents[0].R.size();i++){
            if( Find( parents[1].R ,0,pos, parents[0].R[i] ) == -1 ){
                c1.R.push_back(parents[0].R[i]);
            }
        }

        for(int i=0;i<parents[1].A.size();i++){
            c1.A.push_back(parents[1].A[i]);
        }

        children.push_back(c1);


        for(int i=0;i<=pos;i++){
            c2.R.push_back(parents[0].R[i]);
        }

        for(int i=0;i<parents[1].R.size();i++){
            if( Find( parents[0].R ,0,pos, parents[1].R[i] ) == -1  ){
                c2.R.push_back(parents[1].R[i]);
            }
        }

        for(int i=0;i<parents[0].A.size();i++){
            c2.A.push_back(parents[0].A[i]);
        }

        children.push_back(c2);

        computeFitnessValue(children);

    }

    /* Function to check stopping condition to terminate the loop (done) */
    bool stoppingCondition(int iteration){
        if(iteration > maxIterations ) return true;
        return false;
    }

    /* Function to add the new generated children to the population and replace the old one(done) */
    void addToPopulation( chromosome & ch ){
        population.push_back(ch);
    }

    void addToPopulation( vector<chromosome> & ch ){
        for(int i = 0 ; i < ch.size() ; i++ ){
            population.push_back(ch[i]);
        }
    }

	/* Remove the worst two chromosome from the population */
    void removeFromPopulation(){

        sort( population.begin(),population.end(),cmp2);

        population.pop_back();
        population.pop_back();
    }

	/* Returns the chromosome with the highest fitness value */
    chromosome fittest(){
    	double mx = 0;int idx =0;
    	for(int i=0;i<population.size();i++){
    		if(mx < population[i].fitness){
    			mx = population[i].fitness;
    			idx = i;
    		}
    	}
    	return population[idx];
    }

};


void prn_chromosome(chromosome&ch){
	cout<<1/ch.fitness<<endl;
    for(int i=0;i<ch.R.size();i++){
    	cout<<" ( "<<ch.R[i]<<" , "<<ch.A[i]<<" ) "<<endl;
    }
}

/*Main code begins here*/
int main(){

	srand(time(NULL));

    geneticScheduling G(1000,10,0.2,0.5);

    for(int i=0;i<G.maxPop();i++){
        chromosome c = G.dagToChromosome();
        G.addToPopulation(c);
    }

    int iteration = 1;
    while( G.stoppingCondition(iteration) == false ){

        // Select parents from the population and store them both in a vector named 'parents'
        vector<chromosome> parents;
        G.selectParents(parents);

        // Produce the children from the combination of two parents and store two produced children in vector named 'children'
        vector<chromosome> children;
        G.produceChildren(parents,children);

        // Mutate the children with given mutation rate
        G.mutate(children);

        // Compute the fitness values of the children produced
        G.computeFitnessValue(children);

        // Adding new children to the population and replacing old ones if necessary
        G.addToPopulation(children);

        // To remove the two least fit chromosomes from the population
        if( G.populationSize() > 100 ){
            G.removeFromPopulation();
        }

        iteration++;

    }

    chromosome ch = G.fittest();
    prn_chromosome(ch);
    return 0;
}
