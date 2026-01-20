
// Product slider with animated carousel + fade in/out
document.addEventListener("DOMContentLoaded", function () {
  const leftBtn = document.querySelector(".slider-arrow.left");
  const rightBtn = document.querySelector(".slider-arrow.right");
  const slider = document.querySelector(".product-slider-list");

  if (!leftBtn || !rightBtn || !slider) return;

  let isAnimating = false;
  const animationDuration = 400; // must match CSS

  function moveRight() {
    if (isAnimating) return;
    isAnimating = true;

    const cards = slider.querySelectorAll(".product-slider-card");
    if (cards.length <= 1) return;

    const cardWidth = cards[0].offsetWidth + 30;
    const firstCard = cards[0];

    // Fade out first card
    firstCard.classList.add("fade-out");

    slider.style.transition = "transform 0.4s ease";
    slider.style.transform = `translateX(-${cardWidth}px)`;

    setTimeout(() => {
      slider.style.transition = "none";
      slider.style.transform = "translateX(0)";

      firstCard.classList.remove("fade-out");
      firstCard.classList.add("fade-in");

      slider.appendChild(firstCard);

      // Force fade-in
      requestAnimationFrame(() => {
        firstCard.classList.remove("fade-in");
      });

      isAnimating = false;
    }, animationDuration);
  }

  function moveLeft() {
    if (isAnimating) return;
    isAnimating = true;

    const cards = slider.querySelectorAll(".product-slider-card");
    if (cards.length <= 1) return;

    const cardWidth = cards[0].offsetWidth + 30;
    const lastCard = cards[cards.length - 1];

    // Prepare last card
    lastCard.classList.add("fade-in");
    slider.insertBefore(lastCard, cards[0]);
    slider.style.transition = "none";
    slider.style.transform = `translateX(-${cardWidth}px)`;

    requestAnimationFrame(() => {
      requestAnimationFrame(() => {
        slider.style.transition = "transform 0.4s ease";
        slider.style.transform = "translateX(0)";
        lastCard.classList.remove("fade-in");
      });
    });

    setTimeout(() => {
      isAnimating = false;
    }, animationDuration);
  }

  rightBtn.addEventListener("click", moveRight);
  leftBtn.addEventListener("click", moveLeft);
});

