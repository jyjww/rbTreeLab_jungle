#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void)
{
	rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
	// TODO: initialize struct if needed
	p->nil = (node_t *)calloc(1, sizeof(node_t));
	p->nil->color = RBTREE_BLACK;
	p->nil->parent = p->nil->left = p->nil->right = p->nil;

	p->root = p->nil;

	return p;
}

void delete_rbtree_recur(rbtree *t, node_t *node)
{
	// 모든 노드가 nil이면 리턴. 좌우로 확인하면서 노드 메모리 제거
	if (node == t->nil)
		return;
	delete_rbtree_recur(t, node->left);
	delete_rbtree_recur(t, node->right);
	free(node);
}

void delete_rbtree(rbtree *t)
{
	// TODO: reclaim the tree nodes's memory
	delete_rbtree_recur(t, t->root);
	free(t->nil);	
	free(t);
}

void rbtree_left_rotate(rbtree *t, node_t *x)
{
	node_t *y = x->right;
	// 1. y의 왼쪽 서브트리를 x의 오른쪽으로 옮김
	x->right = y->left;
	if (y->left != t->nil) {
		y->left->parent = x;
	}

	// 2. y의 부모를 x의 부모로 설정
	y->parent = x->parent;

	if (x->parent == t->nil) {
		t->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	// 3. x를 y의 왼쪽 자식으로 설정
	y->left = x;
	x->parent = y;
}

void rbtree_right_rotate(rbtree *t, node_t *y)
{
	node_t *x = y->left;
	// 1. x의 오른쪽 서브트리를 y의 왼쪽으로 옮김
	y->left = x->right;
	if (x->right != t->nil) {
		x->right->parent = y;
	}

	// 2. x의 부모를 y의 부모로 설정
	x->parent = y->parent;

	if (y->parent == t->nil) {
		t->root = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}

	// 3. y를 x의 오른쪽 자식으로 설정
	x->right = y;
	y->parent = x;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
	while (z->parent->color == RBTREE_RED) {

		// z의 부모가 왼쪽 자식인 경우
		if (z->parent == z->parent->parent->left) {
			node_t *u = z->parent->parent->right; // 삼촌 노드

			// case 1: 삼촌도 RED → recoloring
			if (u->color == RBTREE_RED) {
				z->parent->color = RBTREE_BLACK;
				u->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				z = z->parent->parent;
			}
			else {
				// case 2: z가 오른쪽 자식 → 왼쪽 회전하여 CASE 3으로 변환
				if (z == z->parent->right) {
					z = z->parent;
					rbtree_left_rotate(t, z);
				}
				// case 3: z가 왼쪽 자식 → recoloring 후 우회전
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				rbtree_right_rotate(t, z->parent->parent);
			}
		}

		// z의 부모가 오른쪽 자식인 경우 (좌우 반전)
		else {
			node_t *u = z->parent->parent->left; // 삼촌 노드

			// case 1
			if (u->color == RBTREE_RED) {
				z->parent->color = RBTREE_BLACK;
				u->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				z = z->parent->parent;
			}
			else {
				// case 2
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(t, z);
				}
				// case 3
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				rbtree_left_rotate(t, z->parent->parent);
			}
		}
	}

	// 항상 루트는 BLACK
	t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
	// TODO: implement insert
	// 새 노드 메모리 할당 및 초기화
	node_t *new_node = malloc(sizeof(node_t));
	new_node->key = key;
	new_node->color = RBTREE_RED;
	new_node->left = new_node->right = new_node->parent = t->nil;

	// 삽입할 위치를 찾기 위한 포인터들
	node_t *cur = t->root;
	node_t *parent = t->nil;

	// BST 방식으로 삽입할 위치 탐색
	while (cur != t->nil){
		parent = cur;
		if(key < cur->key){
			cur = cur->left;
		}else{
			cur = cur->right;	// 중복 포함하여 오른쪽에 삽입
		}
	}

	//new_node->parent = parent;

	// 트리가 비어 있으면 새 노드를 루트로 지정
	if (parent == t->nil){
		t->root = new_node;
	}
	// 그렇지 않으면 부모의 자식으로 삽입
	if (key < parent->key){
		parent->left = new_node;
	}else{
		parent->right = new_node;
	}

	if (t->root == t->nil) {
		printf("[BUG] 삽입된 key = %d인데 루트가 여전히 nil입니다.\n", key);
	}
	else {
		printf("삽입된 key = %d, 현재 루트 = %d\n", key, t->root->key);
	}

	// 삽입 후 Red-Black Tree 속성 복구
	rbtree_insert_fixup(t, new_node);

	return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *cur = t->root;

  while (cur != t->nil) {
    if (key == cur->key) {
      return cur;
    }
    if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  return NULL;  // 못 찾았으면 NULL 반환
}


node_t *rbtree_min(const rbtree *t)
{
	// TODO: implement find
	if (t->root == t->nil){
		return NULL;
	}
	node_t *cur = t->root;
	while (cur->left != t->nil){
		cur = cur->left;
	}
	return cur;
}


node_t *rbtree_max(const rbtree *t)
{
	// TODO: implement find
	if (t->root == t->nil){
		return NULL;
	}
	node_t *cur = t->root;
	while (cur->right != t->nil){
		cur = cur->right;
	}
	return cur;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
	// u를 v로 대체한다. 만약 u가 루트라면 트리 루트를 갱신한다
	if(u->parent == t->nil){
		t->root = v;
	// u가 왼쪽 자식이면, v를 그 자리에 넣음
	}else if(u == u->parent->left){
		u->parent->left = v;
	// u가 오른쪽 자식이면, v를 넣음
	}else{
		u->parent->right = v;
	}
	// v 부모를 u 부모로 갱신
	v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *p)
{	
	while ((p != t->root) && (p->color == RBTREE_BLACK)){
		// p가 왼쪽 자식인 경우
		if (p == p->parent->left){

			node_t *u = p->parent->right;
			// case 1: 형제가 RED
			if (u->color == RBTREE_RED){
				u->color = RBTREE_BLACK;
				p->parent->color = RBTREE_RED;
				rbtree_left_rotate(t, p->parent);
				u = p->parent->right;
			}
			// case 2: 형제와 형제의 자식 둘 다 BLACK
			if((u->left->color == RBTREE_BLACK) && (u->right->color == RBTREE_BLACK)){
				u->color = RBTREE_RED;
				p = p->parent;
			}else{
				// case 3: 형제는 BLACK, 형제의 오른쪽 자식이 BLACK, 왼쪽 자식이 RED
				if(u->right->color == RBTREE_BLACK){
					if (u->left != t->nil)
						u->left->color = RBTREE_BLACK;
					u->color = RBTREE_RED;
					rbtree_right_rotate(t, u);
					u = p->parent->right;
				}
				// case 4: 형제는 BLACK, 형제의 오른쪽 자식이 RED
				u->color = p->parent->color;
				p->parent->color = RBTREE_BLACK;
				u->right->color = RBTREE_BLACK;
				rbtree_left_rotate(t, p->parent);
				p = t->root;
			}
		// p가 오른쪽 자식인 경우 (좌우 반전)
		}else{
			node_t *u = p->parent->left;
			// case 1
			if(u->color == RBTREE_RED){
				u->color = RBTREE_BLACK;
				p->parent->color = RBTREE_RED;
				rbtree_right_rotate(t, p->parent);
				u = p->parent->left;
			}
			// case 2
			if ((u->right->color == RBTREE_BLACK) && (u->left->color == RBTREE_BLACK)){
				u->color = RBTREE_RED;
				p = p->parent;
			}else{
				// case 3
				if(u->left->color == RBTREE_BLACK){
					if (u->right != t->nil)
						u->right->color = RBTREE_BLACK;
					u->color = RBTREE_RED;
					rbtree_left_rotate(t, u);
					u = p->parent->left;
				}
				// case 4
				u->color = p->parent->color;
				p->parent->color = RBTREE_BLACK;
				u->left->color = RBTREE_BLACK;
				rbtree_right_rotate(t, p->parent);
				p = t->root;
			}
		}
	}
	p->color = RBTREE_BLACK;
}


int rbtree_erase(rbtree *t, node_t *p)
{
	// TODO: implement erase
	if (p == NULL || p == t->nil){
		return -1;
	}

	node_t *y = p;
	node_t *x;
	color_t y_org_color = y->color;
	// case 1: 왼쪽 자식 없음 (오른쪽 자식 or 둘 다 없음)
	if (p->left == t->nil){
		x = p->right;
		rbtree_transplant(t, p, p->right);
	// case 2: 오른쪽 자식 없음 (왼쪽 자식만 있음)
	}else if(p->right == t->nil){
		x = p->left;
		rbtree_transplant(t, p, p->left);
	// case 3: 자식 두명
	}else{
		// 오른쪽 서브트리에서 후임자 찾기 (가장 작은 값)
		y = p->right;
		while(y->left != t->nil){
			y = y->left;
		}
		y_org_color = y->color;
		x = y->right;
		
		// 후임자의 부모가 삭제 대상과 다르면
		if (y->parent != p){
			rbtree_transplant(t, y, y->right);
			y->right = p->right;
			y->right->parent = y;

		// 후임자의 부모가 삭제 대상과 같으면 x의 부모만 재설정
		}else{
			x->parent = y; 
		}

		// 삭제 대상 자리에 후임자 y 배치
		rbtree_transplant(t, p, y);
		y->left = p->left;
		y->left->parent = y;
		y->color = p->color;
	}
	// 삭제 대상이 루트인 경우, 루트를 갱신
	if (p == t->root) {
		if (p->left == t->nil && p->right == t->nil) {
			t->root = t->nil;
		} else if (y != p) {
			t->root = y;
		} else {
			t->root = x;
		}
	}

	// 삭제한 노드가 BLACK일 경우 fixup
	if (y_org_color == RBTREE_BLACK) {
		if (x != t->nil) {
			rbtree_erase_fixup(t, x);
		}
	}

	free(p);
	return 0;
}

/*
size_t inorder_copy(const rbtree *t, node_t *node, key_t *arr, size_t idx, size_t n) {
	if (node == t->nil || idx >= n) return idx;
	idx = inorder_copy(t, node->left, arr, idx, n);
	if (idx < n) arr[idx++] = node->key;
	idx = inorder_copy(t, node->right, arr, idx, n);
	return idx;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
	if (t->root == t->nil) return 0;
	inorder_copy(t, t->root, arr, 0, n);
	return 0;
}
*/

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
	// 트리가 비어있으면 아무 것도 하지 않음
	if (t->root == t->nil) return 0;

	node_t *cur = t->root;
	size_t idx = 0;

	// 중위 순회를 위해 가장 왼쪽 노드까지 이동
	while (cur->left != t->nil) {
		cur = cur->left;
	}

	// 중위 순회 시작
	while (cur != t->nil && idx < n) {
		// 현재 노드의 key를 배열에 저장
		arr[idx++] = cur->key;

		// 오른쪽 자식이 있으면, 그 서브트리의 가장 왼쪽 노드로 이동
		if (cur->right != t->nil) {
			cur = cur->right;
			while (cur->left != t->nil) {
				cur = cur->left;
			}
		}
		// 오른쪽 자식이 없으면, 부모 중 처음으로 현재 노드를 왼쪽 자식으로 가진 부모로 이동
		else {
			while ((cur->parent != t->nil) && (cur == cur->parent->right)) {
				cur = cur->parent;
			}
			cur = cur->parent;
		}
	}
	return 0;
}