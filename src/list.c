
#include  <list.h>




void insertProcQ(pcb_t **head, pcb_t *p){
    /*
    -Checks if there elements in the queue , if not it replaces the first element( Null ) with p
        -If there are elements checks if p needs to be placed before or after the first element
            -Checks if p needs to be placed right after the first element, if not proceedes recursevly on the queue
    */
    if((*head)){

        if(p->p_priority>(*head)->p_priority){

            p->p_next=(*head);
			(*head)=p;
        }
        else{
            if((*head)->p_next==NULL){
                
                (*head)->p_next=p;            
            }
            else{
                if(p->p_priority>(*head)->p_next->p_priority){

                    p->p_next=(*head)->p_next;
                    (*head)->p_next=p;
                }
                else{
                    pcb_t * next= (*head)->p_next;
                    insertProcQ(&next,p);
                }
            }
            
        }
    }
    else
    {
        (*head)=p;
    }

}


pcb_t *headProcQ(pcb_t *head){

	return head;

}


pcb_t *removeProcQ(pcb_t **head){

    // If the queue is empty return NULL , else it deletes the first element and returns it
	if(*head==NULL){
        return NULL;
    }
	else{

		pcb_t *temp=(*head);
		*head=temp->p_next;
		return temp;
	}
}


pcb_t *outProcQ(pcb_t **head, pcb_t *p){

    /*
    -Checks if there elements in the queue , if not it returns NULL
        -If there are elements checks if the first pcb is p, in case it deletes it and returns it
            -Checks if there are elements beside the first one , if not returns NULL
                -Checks if the element right after the first one is p , deletes and returns it if it is , if not proceedes recursevly on the queue
    */
    if((*head)){
        if(p==(*head)){
           (*head)=(*head)->p_next;
            return p;
        }
        else{
            if((*head)->p_next==NULL){
                return NULL;          
            }
            else{
                if(p==(*head)->p_next){
                    (*head)->p_next=(*head)->p_next->p_next;
                    return p;
                }
                else{
                    pcb_t * next= (*head)->p_next;
                    outProcQ(&next,p);
                }
            }
            
        }
    }
    else
    {
        return NULL;
    }

}




void forallProcQ(pcb_t *head, void (*fun)(pcb_t *pcb, void *), void *arg){
    //Calls fun on the current pcb , and proceeds recursevly on the rest of the queue
	if(head){
		fun(head,arg);
		forallProcQ(head->p_next,fun,arg);
	}
	
}
