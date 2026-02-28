const daysEl = document.getElementById("days");
const monthYearEl = document.getElementById("monthYear");
const prevBtn = document.getElementById("prev");
const nextBtn = document.getElementById("next");
const todayBtn = document.getElementById("todayBtn");
let currentDate = new Date();
const resource_id = 1;

const months = [
  "January","February","March","April","May","June",
  "July","August","September","October","November","December"
];

// ----------------------------
// Format date as YYYY-MM-DD
// ----------------------------
function formatDate(year, month, day) {
  const m = String(month + 1).padStart(2, "0");
  const d = String(day).padStart(2, "0");
  return `${year}-${m}-${d}`;
}

// ----------------------------
// Fetch availability (GET)
// ----------------------------
async function loadAvailability(date, resourceId) {
  try {
    const response = await fetch(
      `http://localhost:8000/api/availability?date=${date}&resource_id=${resourceId}`
    );

    const data = await response.json();

    console.log("Availability:", data);

    // Debug only (optional)
    window.open(
      `http://localhost:8000/api/availability?date=${date}&resource_id=${resourceId}`,
      "_blank"
    );

  } catch (err) {
    console.error("Availability error:", err);
  }
}

// ----------------------------
// Render Calendar
// ----------------------------
function renderCalendar() {
  const displayDate = new Date(currentDate);

  const month = displayDate.getMonth();
  const year = displayDate.getFullYear();

  const firstDayIndex = new Date(year, month, 1).getDay();
  const lastDay = new Date(year, month + 1, 0).getDate();
  const prevLastDay = new Date(year, month, 0).getDate();

  monthYearEl.innerText = `${months[month]} ${year}`;
  daysEl.innerHTML = "";

  // Previous month trailing days
  for (let x = firstDayIndex; x > 0; x--) {
    daysEl.innerHTML += `<div class="day inactive">${prevLastDay - x + 1}</div>`;
  }

  // Current month days
  for (let i = 1; i <= lastDay; i++) {
    const fullDate = formatDate(year, month, i);

    const isToday =
      i === new Date().getDate() &&
      month === new Date().getMonth() &&
      year === new Date().getFullYear();

    daysEl.innerHTML += `
      <div 
        class="day current ${isToday ? "today" : ""}" 
        data-date="${fullDate}"
      >
        ${i}
      </div>
    `;
  }
}
// ----------------------------
// Navigation Buttons
// ----------------------------
prevBtn.onclick = () => {
  currentDate.setMonth(currentDate.getMonth() - 1);
  renderCalendar();
};

nextBtn.onclick = () => {
  currentDate.setMonth(currentDate.getMonth() + 1);
  renderCalendar();
};

todayBtn.onclick = () => {
  currentDate = new Date();
  renderCalendar();

  const formatted = formatDate(
    currentDate.getFullYear(),
    currentDate.getMonth(),
    currentDate.getDate()
  );

  loadAvailability(formatted, resource_id);
};

// ----------------------------
// Event Delegation for Days
// ----------------------------
daysEl.addEventListener("click", function (e) {
  const dayEl = e.target.closest(".day.current");
  if (!dayEl) return;

  document.querySelectorAll(".day")
    .forEach(d => d.classList.remove("selected"));

  dayEl.classList.add("selected");

  const selectedDate = dayEl.dataset.date;

  // IMPORTANT: no re-formatting needed
  loadAvailability(selectedDate, resource_id);
});

// ----------------------------
// Initial Render
// ----------------------------

renderCalendar();
