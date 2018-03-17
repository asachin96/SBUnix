#include <master.h>

void createRootFs(){
	file* node = (file *)kmalloc(sizeof(struct file)); 
	kstrcpy(node->name, "rootfs");
	node->first = node->current = node->last = 0;
	node->type  = DIRECTORY;
	node->child[node->last++] = node;
	root->child[root->last++] = node->child[node->last++] = node;
}

void createRoot(){
	root = (file *)kmalloc(sizeof(struct file));
	kstrcpy(root->name, "/");
	root->first = root->current = root->last = 0;
	root->type  = DIRECTORY;
	root->child[root->last++] = root;
	root->child[root->last++] = root; 
}

void* initTarfs(){
	uint64_t ptr = (uint64_t) &_binary_tarfs_start;
	int size;
	createRoot();
	createRootFs();
	while(ptr < (uint64_t)&_binary_tarfs_end){
		tarfsHeader *header  = (tarfsHeader*)ptr;
		size = toDecimal(atoi(header->size));
		file *node, *currnode = root->child[2];
		int type = DIRECTORY;
		uint64_t start = 0, end = 2;
		if (kstrcmp(header->typeflag, "5") != 0) {
			type = FILE;
			start = (uint64_t)(header+1);
			end = (uint64_t)((void *)header + sizeof(tarfsHeader) + size);
		}
		char *temp = kstrtok(header->name, "/");  
		while(temp != NULL) {
			int i=2;
			while(i < currnode->last){
				if(kstrcmp(temp, currnode->child[i]->name) == 0) {
					currnode = (file *)currnode->child[i];
					break;       
				}        
				i++;
			}
			if(i == currnode->last) {
				node = (file *)kmalloc(sizeof(struct file));
				kstrcpy(node->name, temp);
				node->first = node->current= start;
				node->last   = end;
				node->type  = type;
				node->child[0] = currnode->child[currnode->last++] = node;
				node->child[1] = currnode;    
			} 
			temp = kstrtok(NULL, "/");          
		}

		ptr += sizeof(tarfsHeader);
		if (size > 0) {
			ptr += size + sizeof(tarfsHeader) - size % sizeof(tarfsHeader);
		}
	}
	return (void *)&root; 
}

