#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "piecetable.h"

PieceList* newPieceList(){
    PieceList* pl=(PieceList*)malloc(sizeof(PieceList));
    if (!pl)
    {
        return NULL;
    }

    pl->head=NULL;
    pl->tail=NULL;
    pl->all_nodes=newVectorCapa(0,sizeof(bool),128);
    
}

void freePieceList(Vector* all_nodes,Piece* p){
    PieceEdge* pe=NULL;
    while (p->edges)
    {
        pe=p->edges;
        if (GETV(all_nodes,bool,p->edges->next_index_in_all_nodes)==true)// next node is not free
        {
            freePieceList(all_nodes,p->edges->next);
        }
        p->edges=p->edges->next_edge;
        free(pe);
    }
    
    GETV(all_nodes,bool,p->index_in_all_nodes)=false;
    free(p);
}



PieceTable* newPieceTable(FILE* fp){
    PieceTable* piecetable=(PieceTable*)malloc(sizeof(PieceTable));
    if (!piecetable)
    {
        return NULL;
    }

    piecetable->add=newVector(0,sizeof(char));
    piecetable->piece_list=newPieceList();
    piecetable->original_linebreak=newVectorCapa(0,sizeof(long),16);
    piecetable->add_linebreak=newVectorCapa(0,sizeof(long),16);
    // piecetable->now_ver=0;
    piecetable->ver_counter=0;
    
    size_t f_size=0;
    fseek(fp,0,SEEK_END);
    f_size=ftell(fp);
    fseek(fp,0,SEEK_SET);

    piecetable->original=newVector(f_size,sizeof(char));

    Piece* p=(Piece*)malloc(sizeof(Piece));
    p->edges=NULL;
    p->index_in_all_nodes=0;
    piecetable->piece_list->head=piecetable->piece_list->tail=p;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    

    if (f_size!=0)
    {
        fread(piecetable->original->data,sizeof(char),f_size,fp);
        for (size_t i = 0; i < piecetable->original->len; i++)
        {
            if (GETV(piecetable->original,char,i)=='\n')
            {
                PUSHBACKV(piecetable->original_linebreak,long,i);
            }
            
        }

        p->is_inline=(piecetable->original_linebreak->len==0)?true:false;
        
        p->is_add=false;
        p->start=0;
        p->end=f_size-1;
        p->is_empty=false;
        p->line_break_start=0;
        p->line_break_end=piecetable->original_linebreak->len-1;
    }
    else
    {
        p->is_add=true;
        p->start=0;
        p->end=0;
        p->is_empty=true;
        p->line_break_start=0;
        p->line_break_end=0;
    }
    
    return piecetable;
}

void freePieceTable(PieceTable* pt){
    freePieceList(pt->piece_list->all_nodes,pt->piece_list->head);
    freeVector(pt->piece_list->all_nodes);
    free(pt->piece_list);
    freeVector(pt->add);
    freeVector(pt->add_linebreak);
    freeVector(pt->original);
    freeVector(pt->original_linebreak);
    free(pt);
}

Piece* cutPiece(Piece* p, long first_len, PieceTable* piecetable){
    if (first_len>=p->end+1-p->start||p->is_empty)// if first_len==p->len return NULL, try to use insert
    {
        return NULL;
    }
    
    Piece* new_p=(Piece*)malloc(sizeof(Piece));
    PieceEdge* pe=(PieceEdge*)malloc(sizeof(PieceEdge));

    new_p->edges=p->edges;
    new_p->start=p->start+first_len;
    new_p->end=p->end;
    new_p->index_in_all_nodes=piecetable->piece_list->all_nodes->len;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    new_p->is_add=p->is_add;
    new_p->is_empty=false;

    p->end=p->start+first_len-1;

    pe->next=new_p;
    pe->next_edge=NULL;
    pe->next_index_in_all_nodes=p->index_in_all_nodes;
    pe->ver=0;//*some problem?

    p->edges=pe;

    Vector* linebreak=(p->is_add)?(piecetable->add_linebreak):(piecetable->original_linebreak);

    for (size_t i = p->line_break_start;p->is_inline==false && i <= p->line_break_end ; i++)
    {
        if (GETV(linebreak,long,i)>p->end)
        {
            new_p->line_break_end=p->line_break_end;
            p->line_break_end=i-1;
            new_p->line_break_start=i;
            new_p->is_inline=false;
        }

        if (i==p->line_break_end&&GETV(linebreak,long,i)<=p->end)
        {
            new_p->is_inline=true;
            new_p->line_break_end=-1;
            new_p->line_break_start=0;
        }
        
        
    }

    if (p->is_inline)
    {
        new_p->is_inline=true;
    }
    

    return new_p;
}


Piece* findNextPiece(Piece* p, long ver){
    PieceEdge* pe=p->edges;
    while (pe)
    {
        if (pe->ver==ver)
        {
            return pe->next;
        }
        else
        {
            if (pe->next_edge)
            {
                if (pe->next_edge->ver>ver)
                {
                    return pe->next;
                }
                else
                {
                    pe=pe->next_edge;
                }
                
            }
            else
            {
                return pe->next;
            }
            
            
        }
        
    }

    return NULL;
    
    
}



//* have problem
void deletePiece(Piece* fd, PieceTable* piecetable, long ver){
    piecetable->ver_counter++;
    Piece* p=findNextPiece(fd,ver);// fd != NULL
    PieceEdge* pe=(PieceEdge*)malloc(sizeof(PieceEdge));
    PieceEdge* pe_f=fd->edges;
    pe->ver=piecetable->ver_counter;
    pe->next=findNextPiece(p,ver);
    pe->next_index_in_all_nodes=(pe->next)?(pe->next->index_in_all_nodes):(-1);
    pe->next_edge=NULL;

    while (pe_f->next_edge)
    {
        pe_f=pe_f->next_edge;
    }

    pe_f->next_edge=pe;
    
}

Piece* insertPiece(Piece* p,PieceTable* piecetable, long ver){
    piecetable->ver_counter++;
    PieceEdge* pe1=(PieceEdge*)malloc(sizeof(PieceEdge));
    PieceEdge* pe2=(PieceEdge*)malloc(sizeof(PieceEdge));
    PieceEdge* pe_temp=NULL;
    Piece* p_new=(Piece*)malloc(sizeof(Piece));

    p_new->edges=pe2;
    p_new->end=p_new->start=0;
    p_new->line_break_end=p_new->line_break_start=0;
    p_new->index_in_all_nodes=piecetable->piece_list->all_nodes->len;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    p_new->is_add=p->is_add;
    p_new->is_empty=true;
    p_new->is_inline=true;

    pe1->ver=pe2->ver=piecetable->ver_counter;
    pe1->next=p_new;
    pe2->next=findNextPiece(p,ver);
    pe1->next_index_in_all_nodes=p_new->index_in_all_nodes;
    pe2->next_index_in_all_nodes=(pe2->next)?(pe2->next->index_in_all_nodes):-1;
    pe1->next_edge=pe2->next_edge=NULL;

    pe_temp=p->edges;
    while (pe_temp->next_edge)
    {
        pe_temp=pe_temp->next_edge;
    }
    pe_temp->next_edge=pe1;

}


void printPieces(PieceTable* piecetable, FILE* fp, long ver){
    Piece* p=piecetable->piece_list->head;
    char ch=0;
    while (p)
    {
        if (!p->is_empty)
        {
            if (p->is_add)
            {
                for (size_t i = p->start; i <= p->end ; i++)
                {
                    ch=GETV(piecetable->add,char,i);
                    putc(ch,fp);
                    if (ch=='\n'&&fp==stdout)
                    {
                        putc('\r',fp);
                    }
                    
                }
                
            }
            else
            {
                for (size_t i = p->start; i <= p->end; i++)
                {
                    ch=GETV(piecetable->original,char,i);
                    putc(ch,fp);
                    if (ch=='\n'&&fp==stdout)
                    {
                        putc('\r',fp);
                    }
                }
                
            }
            
        }

        p=findNextPiece(p,ver);
        
    }
    
}

long getLineLen(Piece* p, PieceTable* pt, long ver){
    
}


