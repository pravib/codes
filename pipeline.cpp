#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#define T 1
#define NT 0
int gs,bi;
unsigned int cycle=0;
using namespace std;
unsigned int l1ac=0,l2ac=0,l1hit=0,l2hit=0;
void inc(int *);void dec(int *); bool verdict(int*,bool);
class block
{
    public:
    bool valid;
    unsigned int tag;
    bool data[5];
    block()
    {
        valid=0;
        for(int i=0;i<5;i++)
        data[i]=0;
    }
};class block
{
    public:
    bool valid;
    unsigned int tag;
    bool data[7];
    block()
    {
        valid=0;
        for(int i=0;i<7;i++)
        data[i]=0;
    }
};
class set
{
    public:
    block b[2];
    bool flip;
    set()
    {
        flip=0;
    }
};
class set2
{
    public:
    block b[8];
    int t[8];
    set2()
    {
        for(int i=0;i<8;i++)
        t[i]=-1;
    }
};
class L1
{
    public:
    set s[512];
    unsigned int get_tag(unsigned int a)
    {
    return a>>14;
    }
    unsigned int get_ind(unsigned int a)
    {
        a=a<<18;
        return a>>23;
    }
    bool search(unsigned int a)
    {
        int j;
        unsigned int t=this->get_tag(a);
        unsigned int in=this->get_ind(a);
        for(j=0;j<2;j++)
        {
            if((this->s[in].b[j].tag==t)&&(this->s[in].b[j].valid==1)&&(this->s[in].b[j].data[i]==1))
            return 1;
        }
        if(j==2)
        return 0;
    }
    void add(unsigned int a)
    {
        int i;
        unsigned int in=this->get_ind(a);
        unsigned int t=this->get_tag(a);
        for(i=0;i<2;i++)
        {
            if(this->s[in].b[i].valid==0)
            {
                this->s[in].b[i].valid=1;
                this->s[in].b[i].tag=t;
                this->s[in].b[j].data[i]=1;
                return;
            }
            else if((this->s[in].b[i].valid==1)&&(this->s[in].b[i].tag==t))
            {
               return;
            }
        }
        if(i==2)
        {
            this->s[in].b[this->s[in].flip].tag=t;
            this->s[in].flip=~this->s[in].flip;
        }
    }
}l1;
class L2
{
    public:
    set2 s[2048];
    unsigned int get_tag(unsigned int a)
    {
    return a>>18;
    }
    unsigned int get_ind(unsigned int a)
    {
        a=a<<14;
        return a>>21;
    }
    bool search(unsigned int a)
    {
        int j;
        unsigned int t=this->get_tag(a);
        unsigned int in=this->get_ind(a);
        for(j=0;j<8;j++)
        {
            if(this->s[in].b[j].tag==t)
            return 1;
        }
        if(j==8)
        return 0;
    }
    void add(unsigned int a)
    {
        int i;
        unsigned int t=this->get_tag(a);
        unsigned int in=this->get_ind(a);
        for(i=0;i<8;i++)
        {
            if(this->s[in].b[i].valid==0)
            {
                this->s[in].b[i].valid=1;
                this->s[in].b[i].tag=t;
                for(int j=0;j<7;j++)
                this->s[in].t[j]=this->s[in].t[j+1];
                this->s[in].t[7]=i;
                return;
            }
            else if((this->s[in].b[i].valid==1)&&(this->s[in].b[i].tag==t))
            {
                int j;
                for(j=0;j<8;j++)
                {
                    if(this->s[in].t[j]==i)
                    {
                        for(int k=j;k<7;k++)
                        this->s[in].t[k]=this->s[in].t[k+1];
                        this->s[in].t[7]=i;
                        return;
                    }
                }
            }
        }
        if(i==8)
        {
            int k=this->s[in].t[0];
            this->s[in].b[k].tag=t;
            for(int j=0;j<7;j++)
            this->s[in].t[j]=this->s[in].t[j+1];
            this->s[in].t[7]=k;
        }
    }
}l2;
void search(unsigned int a)
{
    l1ac++;
    if(l1.search(a))
    l1hit=l1hit+1;
    else
    {
        l1.add(a);
        l2ac++;
        if(l2.search(a))
        {
            l2hit=l2hit+1;
            cycle=cycle+8;
        }
        else
        {
            l2.add(a);
            cycle=cycle+208;
        }
    }
}
class inst
{
    public:
    unsigned int pc;
    int type;
    int rs;
    int rt;
    int rd;
    int id;
    int taken;
    int mem;
    void fetch_inst();
    void decode();
    void execute();
    void memoryaccess();
    void writeback();
};
class bimodal
{
int arr[1024];
friend void inc(int *);
friend void dec(int *);
friend bool verdict(int *,bool);
public:
bimodal(){for(int i=0;i<1024;i++)arr[i]=1;}
bool predict(unsigned int pc,bool result)
{
    unsigned int k=pc<<22;
    k=k>>22;
    bool b=verdict(&arr[k],result);
	return b;
}
};
class gshare
{
    unsigned int bhr:6;
    int arr[4096];
    friend void inc(int*);
    friend void dec(int*);
    friend bool verdict(int*,bool);
    public:
    gshare(){bhr=0;for(int i=0;i<4096;i++)arr[i]=1;}
    bool predict(unsigned int pc,bool result)
    {
    unsigned int k=pc^bhr;
    k=k<<20;
    k=k>>20;
    bool b=verdict(&arr[k],result);
	return b;
    }
    void trainbhr(bool result)
    {
    bhr=bhr<<1;
    bhr=bhr | result;
    }
};
bool verdict(int *m,bool result)
{
     if((*m)<=1)
    {
        if(NT==result)
        {dec(m);return 1;}
        else {inc(m);return 0;}
    }
    else
    {
        if(T==result)
        {inc(m);return 1;}
        else
		{dec(m);return 0;}
    }
}
void dec(int *m)
{
    if((*m)>0)
    (*m)--;
}
void inc(int*m)
{
    if((*m)<3)
    (*m)++;
}
bool branch(unsigned int pc, bool result,int* accu,gshare*bp1,bimodal*bp2,int*select)
{
    unsigned int k=pc;
    k=k<<20;
    k=k>>20;
    bool b;
    if(select[k]<=1)
    {
        b=bp1->predict(pc,result);
        gs++;
        if(b==1)
        dec(&select[k]);
        else
        inc(&select[k]);
    }
    else
    {
        b= bp2->predict(pc,result);
        bi++;
        if(b==1)
        inc(&select[k]);
        else
        dec(&select[k]);
    }
    if(b==1)
    (*accu)++;
    bp1->trainbhr(result);
	return b;
 }
int main()
{
    gshare bp1;bimodal bp2;
    int select[4096];
    int result;
    for(int i=0;i<4096;i++)select[i]=1;
    int br=0;
    int accu;
    accu=gs=bi=0;
    int exec=0;
     inst pipe;
     inst *temp=NULL;
     unsigned int pc;
     unsigned int mem;
     int taken;
     int n=0;
     FILE* fp;
     inst* ins;
     ins=(inst*)malloc(1000*sizeof(inst));
     int i=0;
     fp=fopen("inst_trace.txt","r");
     if(fp==NULL)
     {
     printf("Unable to read the file as fp==NULL");
     exit(1);
     }
     else
     {
      while(fscanf(fp,"%x",&pc)!=EOF)
         {
            ins[pc].pc=pc;
            fscanf(fp,"%d",&ins[pc].type);
            fscanf(fp,"%d",&ins[pc].rs);
            fscanf(fp,"%d",&ins[pc].rt);
            fscanf(fp,"%d",&ins[pc].rd);
            i++;
         }
     }
      fp=fopen("trace.txt","r");
     if(fp==NULL){
     printf("Unable to read the file");
     exit(1);
     }
 while(fscanf(fp,"%x%x%d",&pc,&mem,&taken)!=EOF)
 {
      n++;
      if(temp!=NULL && temp->type==0)
      {
      if(
      (ins[pc].type==0 && temp->rd==ins[pc].rs)
      || (ins[pc].type==1 && (temp->rd==ins[pc].rs))
       || (ins[pc].type==2 && (temp->rd==ins[pc].rs || temp->rd==ins[pc].rt))
       || (ins[pc].type==3 && (temp->rd==ins[pc].rs))
      )
      cycle++;
      }
      if(ins[pc].type==3)
      {
      br++;
      if(branch(pc,taken,&accu,&bp1,&bp2,select)==0)
      cycle=cycle+2;
      }
      if(ins[pc].type==0)
      search(mem);
      else if(ins[pc].type==1)
      {
        l1ac++;
        if(l1.search(mem))
        l1hit++;
        else
        {
            l1.add(mem);
            l2ac++;
            if(l2.search(mem))
            l2hit++;
            else
            l2.add(mem);
        }
      }
      temp=&ins[pc];
      cycle++;
 }
 cout<<n*1.0/cycle<<" "<<(1-(1.0*l1hit)/l1ac)*100<<"%"<<" "<<(1-(1.0*l2hit)/l2ac)*100<<"%"<<" "<<accu*100.00/br<<"%\n";
 return 0;
}

