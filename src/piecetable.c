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

    pl->head_edges=NULL;
    pl->tail_piece=NULL;
    pl->all_nodes=newVectorCapa(0,sizeof(bool),128);
    
}

void freePieceListR(Vector* all_nodes,Piece* p){
    PieceEdge* pe=NULL;
    while (p->bk_edges)
    {
        pe=p->bk_edges;
        if (pe->next&&GETV(all_nodes,bool,p->bk_edges->next_index_in_all_nodes)==true)// next node is not free
        {
            freePieceListR(all_nodes,p->bk_edges->next);
        }
        p->bk_edges=p->bk_edges->fdp_next_edge;
        free(pe);
    }
    
    GETV(all_nodes,bool,p->index_in_all_nodes)=false;
    free(p);
}

void freePieceList(PieceList* pl){
    PieceEdge* pe=NULL;
    while (pl->head_edges)
    {
        pe=pl->head_edges;
        if (pe->next&&GETV(pl->all_nodes,bool,pl->head_edges->next_index_in_all_nodes)==true)
        {
            freePieceListR(pl->all_nodes,pl->head_edges->next);
        }
        pl->head_edges=pl->head_edges->fdp_next_edge;
        free(pe);
        
    }
    free(pl->all_nodes);
    // free(pl)
    free(pl);
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

    // Piece* p=(Piece*)malloc(sizeof(Piece));
    Piece* tail=(Piece*)malloc(sizeof(Piece));
    // p->bk_edges=NULL;
    // p->fd_edges=NULL;
    tail->bk_edges=NULL;
    tail->fd_edges=NULL;
    tail->index_in_all_nodes=0;
    // p->index_in_all_nodes=0;
    // piecetable->piece_list->head=piecetable->piece_list->tail=p;
    // piecetable->piece_list->head_edges=newPieceEdge(NULL,p,0);
    // newPieceEdge(p,tail,0);
    piecetable->piece_list->tail_piece=tail;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    // PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);

    tail->is_add=false;
    tail->is_empty=false;
    tail->is_inline=true;
    tail->line_break_end=tail->line_break_start=0;

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
        Piece* p=(Piece*)malloc(sizeof(Piece));
        p->is_inline=(piecetable->original_linebreak->len==0)?true:false;
        p->is_add=false;
        p->start=0;
        p->end=f_size-1;
        p->is_empty=false;
        p->line_break_start=0;
        p->line_break_end=piecetable->original_linebreak->len-1;
        p->index_in_all_nodes=1;
        p->bk_edges=p->fd_edges=NULL;
        PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);

        piecetable->piece_list->head_edges=newPieceEdge(NULL,p,0);
        newPieceEdge(p,tail,0);

        PUSHBACKV(piecetable->original,char,' ');
        tail->end=tail->start=piecetable->original->len-1;
    }
    else
    {
        // p->is_add=true;
        // p->start=0;
        // p->end=0;
        // p->is_empty=true;
        // p->line_break_start=0;
        // p->line_break_end=0;
        piecetable->piece_list->head_edges=newPieceEdge(NULL,tail,0);

        PUSHBACKV(piecetable->original,char,' ');
        tail->end=tail->start=piecetable->original->len-1;
    }
    return piecetable;
}

void freePieceTable(PieceTable* pt){
    // freePieceList(pt->piece_list->all_nodes,pt->piece_list->head);
    // freeVector(pt->piece_list->all_nodes);
    freePieceList(pt->piece_list);
    freeVector(pt->add);
    freeVector(pt->add_linebreak);
    freeVector(pt->original);
    freeVector(pt->original_linebreak);
    free(pt);
}

PieceEdge* newPieceEdge(Piece* fdp,Piece* bkp,long ver){
    PieceEdge* pe=(PieceEdge*)malloc(sizeof(PieceEdge));
    PieceEdge* temp=NULL;
    pe->prev=fdp;
    pe->next=bkp;
    pe->ver=ver;
    pe->fdp_next_edge=NULL;
    pe->bkp_next_edge=NULL;
    pe->next_index_in_all_nodes=-1;
    if (fdp!=NULL)
    {
        temp=fdp->bk_edges;
        if (temp!=NULL)
        {
            while (temp->fdp_next_edge)
            {
                temp=temp->fdp_next_edge;
            }
            temp->fdp_next_edge=pe;
        }
        else
        {
            fdp->bk_edges=pe;
        }
    }
    
    
    
    if (bkp!=NULL)
    {
        pe->next_index_in_all_nodes=bkp->index_in_all_nodes;
        temp=bkp->fd_edges;
        if (temp!=NULL)
        {
            while (temp->bkp_next_edge)
            {
                temp=temp->bkp_next_edge;
            }
            temp->bkp_next_edge=pe;
        }
        else
        {
            bkp->fd_edges=pe;
        }
    }
    
    return pe;
    
}

Piece* cutPiece(Piece* p, long first_len, PieceTable* piecetable){
    if (first_len>=p->end+1-p->start||p->is_empty)// if first_len==p->len return NULL, try to use insert
    {
        return NULL;
    }
    
    Piece* new_p=(Piece*)malloc(sizeof(Piece));
    // PieceEdge* pe=(PieceEdge*)malloc(sizeof(PieceEdge));
    
    PieceEdge* temp_pe=p->bk_edges;
    new_p->bk_edges=p->bk_edges;

    while (temp_pe)
    {
        temp_pe->prev=new_p;
        temp_pe=temp_pe->fdp_next_edge;
    }
    

    new_p->fd_edges=NULL;
    new_p->start=p->start+first_len;
    new_p->end=p->end;
    new_p->index_in_all_nodes=piecetable->piece_list->all_nodes->len;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    new_p->is_add=p->is_add;
    new_p->is_empty=false;
    new_p->is_inline=true;
    new_p->line_break_end=new_p->line_break_start=0;

    p->end=p->start+first_len-1;
    p->bk_edges=NULL;

    PieceEdge* pe=newPieceEdge(p,new_p,0);

    Vector* linebreak=(p->is_add)?(piecetable->add_linebreak):(piecetable->original_linebreak);
    

    for (size_t i = p->line_break_start;p->is_inline==false && i <= p->line_break_end ; i++)
    {
        if (GETV(linebreak,long,i)>p->end)
        {
            new_p->line_break_end=p->line_break_end;
            // p->line_break_end=i-1;
            new_p->line_break_start=i;
            new_p->is_inline=false;
            if (p->line_break_start==i)
            {
                p->line_break_end=p->line_break_start=0;
                p->is_inline=true;
            }
            else
            {
                p->line_break_end=i-1;
            }
            
            
        }

        if (i==p->line_break_end&&GETV(linebreak,long,i)<=p->end)
        {
            new_p->is_inline=true;
            new_p->line_break_end=0;
            new_p->line_break_start=0;
        }
        
        
    }

    return new_p;
}


Piece* findNextPiece(Piece* p, long ver){
    PieceEdge* pe=p->bk_edges;
    while (pe)
    {
        if (pe->ver==ver)
        {
            return pe->next;
        }
        else if (pe->ver<ver)
        {
            if (pe->fdp_next_edge)
            {
                if (pe->fdp_next_edge->ver>ver)
                {
                    return pe->next;
                }
                else
                {
                    pe=pe->fdp_next_edge;
                }
                
            }
            else
            {
                return pe->next;
            }
            
        }
        else
        {
            break;
        }

    }

    return NULL;

}

Piece* findNextPieceByPE(PieceEdge* pe, long ver){
    // PieceEdge* pe=p->bk_edges;
    while (pe)
    {
        if (pe->ver==ver)
        {
            return pe->next;
        }
        else if (pe->ver<ver)
        {
            if (pe->fdp_next_edge)
            {
                if (pe->fdp_next_edge->ver>ver)
                {
                    return pe->next;
                }
                else
                {
                    pe=pe->fdp_next_edge;
                }
                
            }
            else
            {
                return pe->next;
            }
            
        }
        else
        {
            break;
        }

    }

    return NULL;

}

Piece* findPrevPiece(Piece* p,long ver){
    PieceEdge* pe=p->fd_edges;
    while (pe)
    {
        if (pe->ver==ver)
        {
            return pe->prev;
        }
        else if (pe->ver<ver)
        {
            if (pe->bkp_next_edge)
            {
                if (pe->bkp_next_edge->ver>ver)
                {
                    return pe->prev;
                }
                else
                {
                    pe=pe->bkp_next_edge;
                }
                
            }
            else
            {
                return pe->prev;
            }
            
        }
        else
        {
            break;
        }

    }

    return NULL;
    
}

Piece* findPrevPieceByPE(PieceEdge* pe,long ver){
    // PieceEdge* pe=p->fd_edges;
    while (pe)
    {
        if (pe->ver==ver)
        {
            return pe->prev;
        }
        else if (pe->ver<ver)
        {
            if (pe->bkp_next_edge)
            {
                if (pe->bkp_next_edge->ver>ver)
                {
                    return pe->prev;
                }
                else
                {
                    pe=pe->bkp_next_edge;
                }
                
            }
            else
            {
                return pe->prev;
            }
            
        }
        else
        {
            break;
        }

    }

    return NULL;
    
}

//* have problem
void deletePiece(Piece* p, PieceTable* piecetable, long ver){
    piecetable->ver_counter++;
    Piece* fdp=findPrevPiece(p,ver);
    Piece* bkp=findNextPiece(p,ver);
    PieceEdge* pe=newPieceEdge(fdp,bkp,piecetable->ver_counter);
    PieceEdge* temp_pe=NULL;
    if (!fdp)// p is head
    {
        temp_pe=piecetable->piece_list->head_edges;
        while (temp_pe->fdp_next_edge)
        {
            temp_pe=temp_pe->fdp_next_edge;
        }
        temp_pe->fdp_next_edge=pe;
        
    }

    if (!bkp)
    {
        piecetable->piece_list->tail_piece=p;
        
    }
    
    
}

Piece* insertAfterPiece(Piece* p,PieceTable* piecetable, long ver){
    piecetable->ver_counter++;
    Piece* p_new=(Piece*)malloc(sizeof(Piece));
    Piece* p_next=findNextPiece(p,ver);
    p_new->end=p_new->start=0;
    p_new->line_break_end=p_new->line_break_start=0;
    p_new->index_in_all_nodes=piecetable->piece_list->all_nodes->len;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    p_new->is_add=true;
    p_new->is_empty=true;
    p_new->is_inline=true;
    p->bk_edges=NULL;

    PieceEdge* pe=newPieceEdge(p_new,p_next,piecetable->ver_counter);
    newPieceEdge(p,p_new,piecetable->ver_counter);
    
    if (pe->next==NULL)
    {
        piecetable->piece_list->tail_piece=p_new;
    }

    return p_new;

}

Piece* insertBeforePiece(Piece* p,PieceTable* piecetable,long ver){
    piecetable->ver_counter++;
    Piece* p_new=(Piece*)malloc(sizeof(Piece));
    Piece* p_prev=findPrevPiece(p,ver);
    p_new->end=p_new->start=0;
    p_new->line_break_end=p_new->line_break_start=0;
    p_new->index_in_all_nodes=piecetable->piece_list->all_nodes->len;
    PUSHBACKV(piecetable->piece_list->all_nodes,bool,true);
    p_new->is_add=true;
    p_new->is_empty=true;
    p_new->is_inline=true;
    p->fd_edges=NULL;

    PieceEdge* pe=newPieceEdge(p_prev,p_new,piecetable->ver_counter);
    newPieceEdge(p_new,p,piecetable->ver_counter);
    
    PieceEdge* temp_pe=NULL;

    if (pe->prev==NULL)// p is head
    {
        temp_pe=piecetable->piece_list->head_edges;
        while (temp_pe->fdp_next_edge)
        {
            temp_pe=temp_pe->fdp_next_edge;
        }
        temp_pe->fdp_next_edge=pe;
        
    }

    return p_new;

}


void printPieces(PieceTable* piecetable, FILE* fp, long ver){
    Piece* p=findNextPieceByPE(piecetable->piece_list->head_edges,ver);
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


