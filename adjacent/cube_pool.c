#include "cube_pool.h"
#include <stdio.h>

static Cube cube_pool[MAX_CUBES];
static Cube *free_list_head = NULL;

void init_cube_pool(void) {
  free_list_head = &cube_pool[0];
  for (int i = 0; i < MAX_CUBES - 1; i++) {
    cube_pool[i].next = &cube_pool[i + 1];
  }
  cube_pool[MAX_CUBES - 1].next = NULL;
}

Cube *alloc_cube(void) {
  if (free_list_head == NULL) {
    fprintf(stderr, "Error: Cube pool is full.\n");
    return NULL;
  }
  Cube *allocated = free_list_head;
  free_list_head = free_list_head->next;
  allocated->next = NULL;
  return allocated;
}

void free_cube(Cube *cube) {
  if (cube == NULL)
    return;
  cube->next = free_list_head;
  free_list_head = cube;
}

void free_cube_list(Cube *head) {
  while (head != NULL) {
    Cube *next = head->next;
    free_cube(head);
    head = next;
  }
}

int count_cubes(const Cube *head) {
  int count = 0;
  const Cube *curr = head;

  while (curr != NULL) {
    count++;
    curr = curr->next;
  }
  return count;
}