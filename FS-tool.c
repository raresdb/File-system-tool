#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

void touch (Dir* parent, char* name) {
	//parser pointer
	File* current_file = parent->head_children_files;
	//parser pointer
	Dir* current_dir = parent->head_children_dirs;
	File* new_file = malloc(sizeof(File));

	new_file->name = strdup(name);
	new_file->next = NULL;
	new_file->parent = parent;

	//checking to make sure the name isn't taken by a directory
	while(current_dir)
	{
		if(!strcmp(current_dir->name, name))
		{
			//freeing the now unnccecessary new file
			free(new_file->name);
			free(new_file);
			printf("File already exists\n");
			return;
		}

		current_dir = current_dir->next;
	}

	//if there is no file in the directory, we are assured the name isn't taken
	if(!parent->head_children_files)
	{
		parent->head_children_files = new_file;
		return;
	}

	//verifying the first file first cause the next loop won't
	if(!strcmp(current_file->name, name))
		{
			free(new_file->name);
			free(new_file);
			printf("File already exists\n");
			return;
		}

	//we need to stop before reaching the NULL pointer
	while(current_file->next)
	{
		if(!strcmp(current_file->name, name))
		{
			free(new_file->name);
			free(new_file);
			printf("File already exists\n");
			return;
		}

		current_file = current_file->next;
	}
	
	//updating the chain by changing the last element's "next" to the new file
	current_file->next = new_file;
}

void mkdir (Dir* parent, char* name) {
	//parser pointer
	File* current_file = parent->head_children_files;
	//parser pointer
	Dir* current_dir = parent->head_children_dirs;
	Dir* new_dir = malloc(sizeof(Dir));

	new_dir->name = strdup(name);
	new_dir->next = NULL;
	new_dir->parent = parent;
	new_dir->head_children_dirs = NULL;
	new_dir->head_children_files = NULL;

	//implementation similar to touch
	while(current_file)
	{
		if(!strcmp(current_file->name, name))
		{
			free(new_dir->name);
			free(new_dir);
			printf("Directory already exists\n");
			return;
		}

		current_file = current_file->next;
	}

	if(!parent->head_children_dirs)
	{
		parent->head_children_dirs = new_dir;
		return;
	}

	if(!strcmp(current_dir->name, name))
		{
			free(new_dir->name);
			free(new_dir);
			printf("Directory already exists\n");
			return;
		}

	while(current_dir->next)
	{
		if(!strcmp(current_dir->name, name))
		{
			free(new_dir->name);
			free(new_dir);
			printf("Directory already exists\n");
			return;
		}

		current_dir = current_dir->next;
	}

	current_dir->next = new_dir;
}

void ls (Dir* parent) {
	//parsers
	Dir* current_dir = parent->head_children_dirs;
	File* current_file = parent->head_children_files;

	//simply parsing and printing each element(directories, then files)
	while(current_dir)
	{
		printf("%s\n", current_dir->name);
		current_dir = current_dir->next;
	}

	while(current_file)
	{
		printf("%s\n", current_file->name);
		current_file = current_file->next;
	}
}

void rm (Dir* parent, char* name) {
	//parser
	File* current_file = parent->head_children_files;

	if(!current_file)
	{
		printf("Could not find the file\n");
		return;
	}

	//first checking if the head of the children files is to be removed
	if(!strcmp(current_file->name, name))
	{
		//before removal, the head must be updated
		parent->head_children_files = current_file->next;
		free(current_file->name);
		free(current_file);
		return;
	}

	//checking the rest of them; keeing the previous file for link updates
	while(current_file->next)
	{
		if(!strcmp(current_file->next->name, name))
		{
			//updating the list by changing the successor of the *previous file*
			File* next = current_file->next;
			current_file->next = next->next;
			free(next->name);
			free(next);
			return;
		}

		current_file = current_file->next;
	}

	printf("Could not find the file\n");
}

//aid function: file lists removers
void __rm_file_list(File* file) {
	//starting with the last element in the chain so we don't lose the head
	if(file->next)
		__rm_file_list(file->next);
	rm(file->parent, file->name);
}

//aid function: directory lists remover(recursive effect)
void __rm_dir_list(Dir* dir) {
	//going for the last element in the chain
	if(dir->next)
		__rm_dir_list(dir->next);
	//cleaning the children too
	if(dir->head_children_dirs)
		__rm_dir_list(dir->head_children_dirs);
	if(dir->head_children_files)
		__rm_file_list(dir->head_children_files);
	free(dir->name);
	free(dir);
}

//aid function: removes the entire tree that has its roots in a given directory
void __rmdir (Dir* dir) {
	if(dir->head_children_dirs)
		__rm_dir_list(dir->head_children_dirs);
	if(dir->head_children_files)
		__rm_file_list(dir->head_children_files);
	free(dir->name);
	free(dir);
}

void rmdir (Dir* parent, char* name) {
	//parser
	Dir* current_dir = parent->head_children_dirs;

	if(!current_dir)
	{
		printf("Could not find the dir\n");
		return;
	}

	if(!strcmp(current_dir->name, name))
	{
		//updating the head
		parent->head_children_dirs = current_dir->next;
		__rmdir(current_dir);
		return;
	}

	while(current_dir->next)
	{
		if(!strcmp(current_dir->next->name, name))
		{
			//updating the chain
			Dir* next = current_dir->next;
			current_dir->next = next->next;
			__rmdir(next);
			return;
		}

		current_dir = current_dir->next;
	}

	printf("Could not find the dir\n");
}

void cd (Dir** target, char *name) {
	//parser
	Dir* current_dir = (*target)->head_children_dirs;

	//back to the parent directory
	if(!strcmp(name, ".."))
	{
		if((*target)->parent)
			*target = (*target)->parent;
		return;
	}

	//looking for the child directory
	while(current_dir)
	{
		if(!strcmp(current_dir->name, name))
		{
			*target = current_dir;
			return;
		}

		current_dir = current_dir->next;
	}

	printf("No directories found!\n");
}

char *pwd (Dir* target) {
	if(target->parent)
	{	
		//place to save the path to the current directory in the recursive parse
		char* partial_path = pwd(target->parent);

		//making space for the current directory's name and updating the path
		partial_path = realloc(partial_path, strlen(partial_path) + 2 + strlen(target->name));
		return strcat(strcat(partial_path, "/"), target->name);
	}

	//only the root directory will reach this block of code
	//--------
	char* name = malloc(2 + strlen(target->name));

	strcpy(name, "/");
	return strcat(name, target->name);
	//--------
}

void stop (Dir* target) {
	//target = root
	__rmdir(target);
}

void tree (Dir* target, int level) {
	//parsers
	Dir* current_dir = target->head_children_dirs;
	File* current_file = target->head_children_files;


	while(current_dir)
	{
		for(int i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", current_dir->name);
		tree(current_dir, level + 1);
		current_dir = current_dir->next;
	}

	while(current_file)
	{
		for(int i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", current_file->name);
		current_file = current_file->next;
	}
}

void mv(Dir* parent, char *oldname, char *newname) {
	//pointer to the file/directory to be changed
	void* target_object;
	
	//parsers
	Dir* current_dir = parent->head_children_dirs;
	File* current_file = parent->head_children_files;

	//"if there are any children directories"
	if(current_dir)
	{
		//case where the target is the first directory
		if(!strcmp(current_dir->name, oldname))
		{
			//special case - avoiding updating the head with a NULL pointer(*)
			if(!current_dir->next)
			{
				strcpy(current_dir->name, newname);
				return;
			}

			target_object = (void*)current_dir;
			//reinitialising the parser to check for newname matches
			current_dir = parent->head_children_dirs;

			while(current_file)
			{
				if(!strcmp(current_file->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}

				current_file=current_file->next;
			}

			//checking the first one first
			if(!strcmp(current_dir->name, newname))
			{
				printf("File/Director already exists\n");
				return;
			}

			//again, we need to stop before reaching the NULL pointer
			while(current_dir->next)
			{
				if(!strcmp(current_dir->next->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}
				current_dir = current_dir->next;
			}

			//end once reached, we can remove, update and reintroduce the dir
			parent->head_children_dirs = parent->head_children_dirs->next;//(*)
			current_dir->next = ((Dir*)target_object);
			strcpy(((Dir*)target_object)->name, newname);
			((Dir*)target_object)->next = NULL;
			return;
		}

		//case where the target is another directory
		while(current_dir->next)
		{

			if(!strcmp(current_dir->next->name, oldname))
			{
				//keeping the directory that is previous to the target
				Dir* previous_dir = current_dir;

				target_object = (void*)current_dir->next;
				//reinitialising the parser
				current_dir = parent->head_children_dirs;

				while(current_file)
				{
					if(!strcmp(current_file->name, newname))
					{
						printf("File/Director already exists\n");
						return;
					}

					current_file=current_file->next;
				}

				if(!strcmp(current_dir->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}

				while(current_dir->next)
				{
					if(!strcmp(current_dir->next->name, newname))
					{
						printf("File/Director already exists\n");
						return;
					}
					current_dir = current_dir->next;
				}

				previous_dir->next = ((Dir*)target_object)->next;
				current_dir->next = ((Dir*)target_object);
				strcpy(((Dir*)target_object)->name, newname);
				((Dir*)target_object)->next = NULL;
				return;
			}

			current_dir = current_dir->next;
		}
	}

	//reinitialising both parsers
	current_file = parent->head_children_files;
	current_dir = parent->head_children_dirs;

	//similar to the "directory branch"
	while(current_file)
	{
		//case where the target is the first file
		if(!strcmp(current_file->name, oldname))
		{
			//special case - we don't want our head to be a NULL pointer
			if(!current_file->next)
			{
				strcpy(current_file->name, newname);
				return;	
			}

			target_object = (void*)current_file;
			current_file = parent->head_children_files;

			while(current_dir)
			{
				if(!strcmp(current_dir->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}

				current_dir=current_dir->next;
			}

			if(!strcmp(current_file->name, newname))
			{
				printf("File/Director already exists\n");
				return;
			}

			while(current_file->next)
			{
				if(!strcmp(current_file->next->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}
				current_file = current_file->next;
			}

			parent->head_children_files = parent->head_children_files->next;
			current_file->next = ((File*)target_object);
			strcpy(((File*)target_object)->name, newname);
			((File*)target_object)->next = NULL;
			return;
		}

		//case where the target is another file
		while(current_file->next)
		{
			if(!strcmp(current_file->next->name, oldname))
			{
				File* previous_file = current_file;

				target_object = (void*)current_file->next;
				current_file = parent->head_children_files;

				while(current_dir)
				{
					if(!strcmp(current_dir->name, newname))
					{
						printf("File/Director already exists\n");
						return;
					}

					current_dir=current_dir->next;
				}

				if(!strcmp(current_file->name, newname))
				{
					printf("File/Director already exists\n");
					return;
				}

				while(current_file->next)
				{
					if(!strcmp(current_file->next->name, newname))
					{
						printf("File/Director already exists\n");
						return;
					}
					current_file = current_file->next;
				}

				previous_file->next = ((File*)target_object)->next;
				current_file->next = ((File*)target_object);
				strcpy(((File*)target_object)->name, newname);
				((File*)target_object)->next = NULL;
				return;
			}

			current_file = current_file->next;
		}
	}

	printf("File/Director not found\n");
}

int main () {

	Dir* root = malloc(sizeof(Dir));

	//the current directory we are in
	Dir* working_dir;
	char* full_command = malloc(MAX_INPUT_LINE_SIZE + 1);
	char* actual_command;

	//the size of the buffer where the full command is stored - needed at (**)
	size_t buffer_size = MAX_INPUT_LINE_SIZE;

	root->name = malloc(sizeof("home"));
	strcpy(root->name, "home");
	root->parent = NULL;
	root->head_children_files = NULL;
	root->head_children_dirs = NULL;
	root->next = NULL;
	working_dir = root;

	do
	{
		getline(&full_command, &buffer_size, stdin);//(**)
		actual_command = strtok(full_command, " ");

		if(!strcmp(actual_command, "touch"))
			touch(working_dir, strtok(NULL, "\n"));
		else if(!strcmp(actual_command, "mkdir"))
			mkdir(working_dir, strtok(NULL, "\n"));
		else if(!strcmp(actual_command, "ls\n"))
			ls(working_dir);
		else if(!strcmp(actual_command, "rm"))
			rm(working_dir, strtok(NULL, "\n"));
		else if(!strcmp(actual_command, "rmdir"))
			rmdir(working_dir, strtok(NULL, "\n"));
		else if(!strcmp(actual_command, "cd"))
			cd(&working_dir, strtok(NULL, "\n"));
		else if(!strcmp(actual_command, "tree\n"))
			tree(working_dir, 0);
		else if(!strcmp(actual_command, "pwd\n"))
		{
			char* name = pwd(working_dir);
			printf("%s\n", name);
			free(name);
		}
		else if(!strcmp(actual_command, "stop\n"))
			stop(root);
		else if(!strcmp(actual_command, "mv"))
		{
			char* oldname = strtok(NULL, " ");
			char* newname = strtok(NULL, "\n");
			mv(working_dir, oldname, newname);
		}
		
	} while (strcmp(actual_command, "stop\n"));
	
	free(full_command);
	return 0;
}
