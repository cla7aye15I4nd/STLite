if (x == nullptr) return newnode(key, val, RED);
            
int cmp = compare(key, x->key);
            
if (cmp == 0) x->value = val;
 else if (cmp < 0) {
     x->left = insert(x->left, key, val, false);
     if (isRed(x) && isRed(x->left) && sw)
         x = rotateRight(x);
     if (isRed(x->left) && isRed(x->left->left)) {
         x = rotateRight(x);
         x->color = BLACK; x->right->color = RED;
     }
 }
 else {
     x->right = insert(x->right, key, val, true);
     if (isRed(x) && isRed(x->right) && !sw)
         x = rotateLeft(x);
     if (isRed(x->right) && isRed(x->right->right)) {
         x = rotateLeft(x);
         x->color = BLACK; x->left->color = RED;
     }
 }
            
if (isRed(x->left) && isRed(x->right))
    colorFlip(x);
            
return x;

private Node delete(Node h, Key key) {
    int cmp = key.compareTo(h.key);
    if (cmp < 0) {
        if (!isRed(h.left) && !isRed(h.left.left))
            h = moveRedLeft(h);
        h.left = delete(h.left, key);
    }
    else {
        if (isRed(h.left)) h = leanRight(h);
        if (cmp == 0 && (h.right == null))
            return null; 
        if (!isRed(h.right) && !isRed(h.right.left))
            h = moveRedRight(h); 
        if (cmp == 0) {
            h.key = min(h.right);
            h.value = get(h.right, h.key);
            h.right = deleteMin(h.right);
        }
        else h.right = delete(h.right, key);
    }
    return fixUp(h);
}
