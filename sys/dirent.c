#include <dirent.h>
#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/virt_mm.h>
#include <sys/kstring.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/util.h>

fnode_t* get_root_node()
{
								return root_node;
}

int findNodeWithFileName(int* index, const  char*temp,fnode_t*currnode)
{
								if (kstrcmp(temp, currnode->f_child[(*index)]->f_name) == 0) {
																currnode = (fnode_t *)currnode->f_child[(*index)];
																return 0;       
								}        
								return 1;
}
void* file_lookup(char *dir_path)
{

								char* file_path = (char *)dir_path;

								fnode_t *aux_node, *currnode = root_node;

								char *temp = NULL; 
								int i;
								char *path = (char *)kmalloc(sizeof(char) * kstrlen(file_path));
								kstrcpy(path, file_path); 

								temp = kstrtok(path, "/");  

								if (temp == NULL)
																return NULL;

								int j=0;
								for (;temp != NULL;j++) {

																aux_node = currnode;
																i=2;
																while(i < currnode->end) {
																								if (kstrcmp(temp, currnode->f_child[i]->f_name) == 0) {
																																currnode = (fnode_t *)currnode->f_child[i];
																																//return 0; 		
																																break;      
																								}    
																								i++;
																}

																if (i == aux_node->end) {
																								return NULL;
																}

																temp = kstrtok(NULL, "/");          
								}

								if (currnode->f_type == FILE)
																return (void *)currnode->start; 
								else
																return NULL;
}

void make_node(struct file *node, struct file *parent, char *name, uint64_t start, uint64_t end, int type, uint64_t inode_no)
{
								kstrcpy(node->f_name, name);
								node->start = start;
								node->end   = end;
								node->curr  = start;
								node->f_type  = type;
								node->f_inode_no = inode_no;

								node->f_child[0] = node;
								node->f_child[1] = parent;    

}

void parse(char *dir_path, int type, uint64_t start, uint64_t end)
{
								fnode_t *temp_node, *aux_node, *currnode = root_node->f_child[2];
								char *temp; 
								int i = 0;

								char *path = (char *)kmalloc(sizeof(char) * kstrlen(dir_path));
								kstrcpy(path, dir_path); 

								temp = kstrtok(path, "/");  
								int j=0;
								//  while (temp != NULL) {
								for (;temp != NULL;j++) {
																aux_node = currnode; 
																//  for(i = 2; i < currnode->end; ++i){
																i=2;
																while( i < currnode->end){
																								if(kstrcmp(temp, currnode->f_child[i]->f_name) == 0) {
																																currnode = (fnode_t *)currnode->f_child[i];
																																break;       
																								}        
																								i++;
																}

																if (i == aux_node->end) {

																								temp_node = (fnode_t *)kmalloc(sizeof(struct file));
																								make_node(temp_node, currnode, temp, start, end, type, 0);  

																								currnode->f_child[currnode->end] = temp_node;
																								currnode->end += 1; 
																} 


																temp = kstrtok(NULL, "/");          

								}
								}

								void printnode(fnode_t *node)
								{
																int i = 0; 

																kprintf("\n%s\t%p", node->f_name, &node);

																for (i = 2; i < node->end; ++i) {
																								if (node->f_child[i]->f_type == FILE) {
																																kprintf("\nfile %s \t %p \t %p", node->f_child[i]->f_name, node->f_child[i]->start, node->f_child[i]->end);
																								} else { 
																																printnode(node->f_child[i]); 
																								}
																}
								}


								void createRootFs(fnode_t*temp_node)
								{
																temp_node = (fnode_t *)kmalloc(sizeof(struct file)); 
																make_node(temp_node, root_node, "rootfs", 0, 2, DIRECTORY, 0);
																root_node->f_child[2] = temp_node; 
																root_node->end += 1;
								}

								void createRoot()
								{
																root_node = (fnode_t *)kmalloc(sizeof(struct file));
																make_node(root_node, root_node, "/", 0, 2, DIRECTORY, 0);  
								}

								void createDisk(fnode_t*temp_node)
								{
																temp_node = (fnode_t *)kmalloc(sizeof(struct file)); 
																make_node(temp_node, root_node, "Disk", 0, 2, DIRECTORY, 0);
																root_node->f_child[3] = temp_node; 
																root_node->end += 1;
								}

								void* init_tarfs()
								{
																HEADER *header = (HEADER*) &_binary_tarfs_start;
																int size_of_file = 0;
																fnode_t *temp_node = NULL;
																createRoot();
																createRootFs(temp_node);	
																createDisk(temp_node);

																do {
																								size_of_file = oct_to_dec(atoi(header->size));

																								if (kstrcmp(header->typeflag, "5") == 0) {
																																parse(header->name, DIRECTORY, 0, 2);
																								} else {
																																parse(header->name, FILE, (uint64_t)(header + 1), (uint64_t)((void *)header + 512 + size_of_file));
																								}

																								if (size_of_file > 0) {
																																header = header + size_of_file / (sizeof(HEADER) + 1) + 2;
																								} else {
																																header = header + 1;
																								}
																} while(header < (HEADER*)&_binary_tarfs_end);

																return (void *)&root_node; 
								}

