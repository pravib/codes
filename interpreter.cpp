#include<iostream>
#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
class node
{
   public:
   int flag;
   char s[10];
   node*next;
   node*down;
   node()
   {
   next=down=NULL;
   }
};
class value
{
    public:
    int tag;
    node*p;
    float f;
    value()
    {
        p=NULL;
    }
};
class env
{
    public:
   env*parent;
   value*a[26];
   env()
  {
    for(int i=0;i<26;i++)
    a[i]=NULL;
    parent=NULL;
  }
};
void addtoenv(char x,value v,env&e)
{
    value*v1;
    v1=new value;
    v1->tag=v.tag;
    v1->f=v.f;
    v1->p=v.p;
    e.a[x-'a']=v1;
}
value findinenv(char x,env&e)
{
    if(e.a[x-'a']!=NULL)
    return *e.a[x-'a'];
    else
    {
        if(e.parent==NULL)
        cout<<x<<" not found ";
        else
        return findinenv(x,*e.parent);
    }
}
value evalnode(node*p,env&e);
value evallist(node*p,env&e)
{
    if(!strcmp(p->s,"begin"))
    {
        p=p->next;
        while(p->next!=NULL)
        {
            evalnode(p,e);
            p=p->next;
        }
        return evalnode(p,e);
    }
    else if(!strcmp(p->s,"define"))
    {
        value v=evalnode(p->next->next,e);
        v.tag=0;
        addtoenv(p->next->s[0],v,e);
        return v;
    }
    else if(!strcmp(p->s,"set"))
    {
        value v;
        v=evalnode(p->next->next,e);
        (e.a[p->next->s[0]-'a'])->f=v.f;
        v.tag=0;
        return v;
    }
    else if(!strcmp(p->s,"if"))
    {
        value v=evalnode(p->next,e);
        if(v.f==1)
        return evalnode(p->next->next,e);
        else
        return evalnode(p->next->next->next,e);
    }
    else if(!strcmp(p->s,"while"))
    {
        value v=evalnode(p->next,e);
        while(v.f==1)
        {
            evalnode(p->next->next,e);
            v=evalnode(p->next,e);
        }
        return v;
    }
    else if (!strcmp(p->s,"lambda"))
    {
    value v;
    v.tag=1;
    v.p=p;
    return v;
    }
    else if(!strcmp(p->s,"+"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        v.f=v1.f+v2.f;
        return v;
    }
    else if(!strcmp(p->s,"-"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        v.f=v1.f-v2.f;
        return v;
    }
    else if(!strcmp(p->s,"*"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        v.f=v1.f*v2.f;
        return v;
    }
    else if(!strcmp(p->s,"/"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        v.f=v1.f/v2.f;
        return v;
    }
    else if(!strcmp(p->s,"%"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        v.f=((int)v1.f)%((int)v2.f);
        return v;
    }
    else if(!strcmp(p->s,"<"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        if(v1.f<v2.f)
        v.f=1;
        else
        v.f=0;
        return v;
    }
    else if(!strcmp(p->s,"<="))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        if(v1.f<=v2.f)
        v.f=1;
        else
        v.f=0;
        return v;
    }
    else if(!strcmp(p->s,">"))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        if(v1.f>v2.f)
        v.f=1;
        else
        v.f=0;
        return v;
    }
    else if(!strcmp(p->s,"=="))
    {
        value v;
        value v1=evalnode(p->next,e);
        value v2=evalnode(p->next->next,e);
        if(v1.f==v2.f)
        v.f=1;
        else
        v.f=0;
        return v;
    }
    else if(strlen(p->s)==1&&isalpha(p->s[0]))
    {
    env e1;
    e1.parent=&e;
    env*temp=&e1;
    while(temp->parent!=NULL)
    {
    if(temp->a[p->s[0]-'a']!=NULL)
    break;
    temp=temp->parent;
    }
    node*arg=temp->a[p->s[0]-'a']->p->next->down;
    node*body=temp->a[p->s[0]-'a']->p->next->next;
    node*call=p->next;
    while(arg!=NULL&&call!=NULL)
    {
    addtoenv(arg->s[0],evalnode(call,e),e1);
    arg=arg->next;
    call=call->next;
    }
    return evalnode(body,e1);
    }
    else if(!strcmp(p->down->s,"lambda"))
    {
    env e2;
    node*arg=p->down->next->down;
    node*body=p->down->next->next;
    node*call=p->next;
    while(arg!=NULL&&call!=NULL)
    {
    addtoenv(arg->s[0],evalnode(call,e),e2);
    arg=arg->next;
    call=call->next;
    }
    return evalnode(body,e2);
    }
    else if(p->flag==1)
    {
        return evallist(p->down,e);
    }
}
value evalnode(node*p,env&e)
{
    if(isdigit(p->s[0]))
    {
        value v;
        v.f=atof(p->s);
        return v;
    }
    else if(isalpha(p->s[0]))
    return findinenv(p->s[0],e);
    else if(p->flag==1)
    return evallist(p->down,e);
}
node*makelist()
{
    char c;
    char str[10];
    node*head=NULL;
    node*curr=NULL;
    while((c=getchar())!='\n')
    {
    if(c==' ')
   {
    cin>>str;
    if(!strcmp(str,"("))
    {
        if(head==NULL)
        {
        head=new node;
        head->flag=1;
        head->down=makelist();
        curr=head;
        }
        else
        {
        curr->next=new node;
        curr=curr->next;
        curr->flag=1;
        curr->down=makelist();
        }
    }
    else if(!strcmp(str,")"))
    return head;
    else
    {
    if(head==NULL)
    {
        head=new node;
        head->flag=0;
        strcpy(head->s,str);
        curr=head;
    }
    else
    {
    curr->next=new node;
    curr=curr->next;
    curr->flag=0;
    strcpy(curr->s,str);
    }
    }
    }
  }
}
int main()
{
node*head=makelist();
env e;
value v=evallist(head,e);
cout<<v.f;
return 0;
}
