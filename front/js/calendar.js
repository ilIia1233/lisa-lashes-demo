document.addEventListener("DOMContentLoaded", function () {

const daysEl = document.getElementById("days");
const monthYearEl = document.getElementById("monthYear");
const prevBtn = document.getElementById("prev");
const nextBtn = document.getElementById("next");
const todayBtn = document.getElementById("todayBtn");

const timeSlotsEl = document.getElementById("timeSlots");

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
// Render Time Slots from API data
// ----------------------------
function renderTimeSlots(slots) {
  timeSlotsEl.innerHTML = "";

  if (!slots || slots.length === 0) {
    timeSlotsEl.innerHTML = `<p class="no-slots">No slots available for this date.</p>`;
    return;
  }

  slots.forEach(slot => {
    const btn = document.createElement("button");
    btn.textContent = slot.start;

    if (slot.free) {
      btn.className = "time-slot";
      btn.addEventListener("click", () => {
        timeSlotsEl.querySelectorAll(".time-slot").forEach(b => b.classList.remove("selected"));
        btn.classList.add("selected");
      });
    } else {
      btn.className = "time-slot booked";
    }

    timeSlotsEl.appendChild(btn);
  });
}

// ----------------------------
// Fetch availability (GET)
// ----------------------------
async function loadAvailability(date, resourceId) {
  timeSlotsEl.innerHTML = `<p class="no-slots">Loading...</p>`;
  try {
    const response = await fetch(
      `http://localhost:8000/api/availability?date=${date}&resource_id=${resourceId}&_=${Date.now()}`
    );

    const data = await response.json();

    console.log("Availability:", data);

    renderTimeSlots(data.slots);


    window.open(
      `http://localhost:8000/api/availability?date=${date}&resource_id=${resourceId}&_=${Date.now()}`,
      "_blank"
    );

  } catch (err) {
    console.error("Availability error:", err);
    timeSlotsEl.innerHTML = `<p class="no-slots">Failed to load availability.</p>`;
  }
}

// ----------------------------
// Render Calendar
// ----------------------------
function renderCalendar() {
  currentDate.setDate(1);

  const month = currentDate.getMonth();
  const year = currentDate.getFullYear();

  const firstDayIndex = currentDate.getDay();
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



  loadAvailability(selectedDate, resource_id);
});

// ----------------------------
// Initial Render
// ----------------------------

renderCalendar();

}); // end DOMContentLoaded

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
