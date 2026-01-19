// Dynamically load HTML partials into the main document
const sections = [
  { id: 'header-section', file: 'sections/header.html' },
  { id: 'promo-section', file: 'sections/promo.html' },
  { id: 'welcome-section', file: 'sections/welcome.html' },
  { id: 'testimonials-section', file: 'sections/testimonials.html' },
  { id: 'trustpilot-section', file: 'sections/trustpilot.html' },
  { id: 'products-section', file: 'sections/products.html' },
  { id: 'booking-section', file: 'sections/booking.html' },
  { id: 'footer-section', file: 'sections/footer.html' }
];

sections.forEach(({ id, file }) => {
  const container = document.getElementById(id);
  if (container) {
    fetch(file)
      .then(res => res.text())
      .then(html => { container.innerHTML = html; });
  }
});