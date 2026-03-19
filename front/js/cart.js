/*

// cart.js - Handles cart API integration for shop and checkout pages

const API_BASE = '/api/cart';

// Utility: Fetch current cart
async function fetchCart() {
  const res = await fetch(API_BASE, { credentials: 'include' });
  if (!res.ok) throw new Error('Failed to fetch cart');
  return await res.json();
}

// Utility: Add item to cart
async function addToCart(productId, quantity = 1) {
  const res = await fetch(`${API_BASE}/items`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    credentials: 'include',
    body: JSON.stringify({ product_id: productId, quantity })
  });
  if (!res.ok) throw new Error('Failed to add to cart');
  return await res.json();
}

// Utility: Update item quantity
async function updateCartItem(productId, quantity) {
  const res = await fetch(`${API_BASE}/items`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    credentials: 'include',
    body: JSON.stringify({ product_id: productId, quantity })
  });
  if (!res.ok) throw new Error('Failed to update cart item');
  return await res.json();
}

// Utility: Remove item from cart
async function removeCartItem(productId) {
  const res = await fetch(`${API_BASE}/items?product_id=${productId}`, {
    method: 'DELETE',
    credentials: 'include'
  });
  if (!res.ok) throw new Error('Failed to remove cart item');
  return await res.json();
}

// Utility: Checkout
async function checkoutCart() {
  const res = await fetch(`${API_BASE}/checkout`, {
    method: 'POST',
    credentials: 'include'
  });
  if (!res.ok) throw new Error('Checkout failed');
  return await res.json();
}

// Expose globally for inline event handlers (optional)
window.cartApi = {
  fetchCart,
  addToCart,
  updateCartItem,
  removeCartItem,
  checkoutCart
};
*/