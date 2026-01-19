// Loop product cards on arrow click (carousel effect)
document.addEventListener('DOMContentLoaded', function() {
  const leftBtn = document.querySelector('.slider-arrow.left');
  const rightBtn = document.querySelector('.slider-arrow.right');
  const slider = document.querySelector('.product-slider-list');
  if (!leftBtn || !rightBtn || !slider) return;

  function moveLeft() {
    // Move last card to the front
    const cards = slider.querySelectorAll('.product-slider-card');
    if (cards.length > 1) {
      slider.insertBefore(cards[cards.length - 1], cards[0]);
    }
  }

  function moveRight() {
    // Move first card to the end
    const cards = slider.querySelectorAll('.product-slider-card');
    if (cards.length > 1) {
      slider.appendChild(cards[0]);
    }
  }

  leftBtn.addEventListener('click', moveLeft);
  rightBtn.addEventListener('click', moveRight);
});
