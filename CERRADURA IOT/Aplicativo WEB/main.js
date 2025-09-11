// --- i18n & Language Configuration ---
const i18n = {
    es: {
        appTitle: "Panel de Administración",
        loginTitle: "Acceso de Administrador",
        loginButton: "Iniciar Sesión",
        logoutButton: "Cerrar Sesión",
        adminViewButton: "Administradores",
        logsViewButton: "Registros",
        searchPlaceholder: "Buscar por nombre o UID...",
        addAdminButton: "Añadir Nuevo",
        modalTitle: "Añadir nueva Llave",
        modalDescription: "Introduce el nombre y el UID de la llave que quieres registrar.",
        modalNamePlaceholder: "Nombre del propietario",
        modalUidPlaceholder: "UID de la Llave",
        cancelButton: "Cancelar",
        saveButton: "Guardar",
        deleteModalTitle: "Confirmar Eliminación",
        deleteModalText: (name) => `¿Estás seguro de que deseas eliminar a "${name}"?`,
        deleteButton: "Eliminar",
        // Dynamic text
        statusActive: "Activo",
        statusInactive: "Inactivo",
        noAdminsFound: "No se encontraron administradores.",
        noLogsFound: "No hay registros de acceso.",
        logCountSuffix: "accesos",
        errorCredentials: "Error: Credenciales incorrectas.",
        errorBothFields: "Ambos campos son obligatorios.",
        errorSave: "Error al guardar.",
        errorLoginFields: "Por favor, introduce el correo y la contraseña."
    }
};
const applyText = (lang) => {
    const texts = i18n[lang];
    document.getElementById('app-title').textContent = texts.appTitle;
    document.getElementById('login-title').textContent = texts.loginTitle;
    document.getElementById('login-button').textContent = texts.loginButton;
    document.getElementById('logout-button').textContent = texts.logoutButton;
    document.getElementById('admin-view-button').textContent = texts.adminViewButton;
    document.getElementById('logs-view-button').textContent = texts.logsViewButton;
    document.getElementById('search-input').placeholder = texts.searchPlaceholder;
    document.getElementById('add-admin-button').textContent = texts.addAdminButton;
    document.getElementById('modal-title').textContent = texts.modalTitle;
    document.getElementById('modal-description').textContent = texts.modalDescription;
    document.getElementById('new-admin-name').placeholder = texts.modalNamePlaceholder;
    document.getElementById('new-admin-uid').placeholder = texts.modalUidPlaceholder;
    document.getElementById('cancel-add-admin').textContent = texts.cancelButton;
    document.getElementById('save-admin-button').textContent = texts.saveButton;
    document.getElementById('cancel-delete-button').textContent = texts.cancelButton;
    document.getElementById('confirm-delete-button').textContent = texts.deleteButton;
    emailInput.placeholder = "Correo electrónico";
    passwordInput.placeholder = "Contraseña";
};

// --- Firebase Configuration ---
const firebaseConfig = {
    apiKey: "AIzaSyBRjLvZXhYDIDReDrYwINRXNKiUJuECYyA",
    authDomain: "registro-inlock.firebaseapp.com",
    databaseURL: "https://registro-inlock-default-rtdb.firebaseio.com",
    projectId: "registro-inlock",
    storageBucket: "registro-inlock.firebasestorage.app",
    messagingSenderId: "509105784063",
    appId: "1:509105784063:web:e84fbcfb0fc49de4a978ca",
    measurementId: "G-Q8P3SN8DF8"
};
let app; try { app = firebase.app(); } catch (e) { app = firebase.initializeApp(firebaseConfig); }
const auth = firebase.auth();
const db = firebase.firestore();

// --- DOM Elements ---
const loginContainer = document.getElementById('login-container');
const appContainer = document.getElementById('app-container');
const loginButton = document.getElementById('login-button');
const logoutButton = document.getElementById('logout-button');
const emailInput = document.getElementById('email');
const passwordInput = document.getElementById('password');
const loginError = document.getElementById('login-error');
const adminView = document.getElementById('admin-view');
const logsView = document.getElementById('logs-view');
const adminViewButton = document.getElementById('admin-view-button');
const logsViewButton = document.getElementById('logs-view-button');
const adminListContainer = document.getElementById('admin-list');
const accessLogsListContainer = document.getElementById('access-logs-list');
const searchInput = document.getElementById('search-input');
const addAdminButton = document.getElementById('add-admin-button');
const modalBackdrop = document.getElementById('modal-backdrop');
const newAdminNameInput = document.getElementById('new-admin-name');
const newAdminUidInput = document.getElementById('new-admin-uid');
const modalError = document.getElementById('modal-error');
const cancelAdminButton = document.getElementById('cancel-add-admin');
const saveAdminButton = document.getElementById('save-admin-button');
const deleteModalBackdrop = document.getElementById('delete-modal-backdrop');
const deleteModalTitle = document.getElementById('delete-modal-title');
const deleteModalText = document.getElementById('delete-modal-text');
const cancelDeleteButton = document.getElementById('cancel-delete-button');
const confirmDeleteButton = document.getElementById('confirm-delete-button');


// --- State ---
let adminDataCache = [];
let itemToDelete = { id: null, name: null };

// --- Auth & Admin Management ---
const performLogin = () => {
    const email = emailInput.value; const password = passwordInput.value;
    loginError.textContent = '';
    if (!email || !password) { loginError.textContent = i18n.es.errorLoginFields; return; }
    auth.signInWithEmailAndPassword(email, password).catch(err => { loginError.textContent = i18n.es.errorCredentials; });
};
const performLogout = () => auth.signOut();
const updateAdminStatus = (uid, newStatus) => db.collection("administrador").doc(uid).update({ Estado: newStatus });
const performDeleteAdmin = async (id) => {
    try {
        await db.collection("administrador").doc(id).delete();
    } catch (error) {
        console.error("Error deleting admin: ", error);
    }
};
const renderAdminList = (data) => {
    adminListContainer.innerHTML = '';
    if (data.length === 0) { adminListContainer.innerHTML = `<p>${i18n.es.noAdminsFound}</p>`; return; }
    data.sort((a, b) => a.data.Nombre.localeCompare(b.data.Nombre));
    data.forEach(({ id, data: admin }) => {
        const card = document.createElement('div');
        card.className = 'simple-card';
        const statusText = admin.Estado === 1 ? i18n.es.statusActive : i18n.es.statusInactive;
        const statusColor = admin.Estado === 1 ? 'var(--secondary-color)' : 'var(--error-color)';
        card.style.borderColor = statusColor;
        card.innerHTML = `
            <div class="details">
                <p class="name">${admin.Nombre}</p>
                <p class="uid">UID: ${admin.UID}</p>
            </div>
            <span class="status-badge" style="background-color: ${statusColor};">${statusText}</span>
            <label class="switch"><input type="checkbox" ${admin.Estado === 1 ? 'checked' : ''}><span class="slider"></span></label>
            <button class="delete-button"><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="3 6 5 6 21 6"></polyline><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path></svg></button>
        `;
        card.querySelector('input[type="checkbox"]').addEventListener('change', (e) => updateAdminStatus(id, e.target.checked ? 1 : 0));
        card.querySelector('.delete-button').addEventListener('click', () => openDeleteConfirmModal(id, admin.Nombre));
        adminListContainer.appendChild(card);
    });
};

const renderLogList = (logs) => {
    accessLogsListContainer.innerHTML = '';
    if (logs.length === 0) { accessLogsListContainer.innerHTML = `<p>${i18n.es.noLogsFound}</p>`; return; }

    const groupedLogs = logs.reduce((acc, log) => {
        const date = log.fecha;
        const name = log.nombre;
        if (!acc[date]) { acc[date] = { logs: {}, total: 0 }; }
        if (!acc[date].logs[name]) { acc[date].logs[name] = []; }
        acc[date].logs[name].push(log.hora);
        acc[date].total++;
        return acc;
    }, {});

    for (const date in groupedLogs) {
        const dateGroup = groupedLogs[date];
        const dateContainer = document.createElement('div');
        dateContainer.className = 'log-group';
        dateContainer.innerHTML = `
            <div class="log-group-header">
                <h3>${date}</h3>
                <span class="log-count-badge">${dateGroup.total} ${i18n.es.logCountSuffix}</span>
            </div>
        `;
        
        const userEntriesContainer = document.createElement('div');
        for (const name in dateGroup.logs) {
            const hours = dateGroup.logs[name];
            const userEntry = document.createElement('div');
            userEntry.className = 'log-entry';
            userEntry.innerHTML = `
                <p class="name">${name}</p>
                <div class="timestamp-list">
                    ${hours.map(h => `<span class="timestamp-item">${h}</span>`).join('')}
                </div>
            `;
            userEntry.addEventListener('click', () => userEntry.classList.toggle('expanded'));
            userEntriesContainer.appendChild(userEntry);
        }
        dateContainer.appendChild(userEntriesContainer);
        accessLogsListContainer.appendChild(dateContainer);
    }
};

// --- Data Fetching & Listeners ---
const fetchAndListenAdmins = () => {
    db.collection("administrador").onSnapshot((snapshot) => {
        adminDataCache = snapshot.docs.map(doc => ({ id: doc.id, data: doc.data() }));
        handleSearch({ target: { value: searchInput.value } });
    }, err => console.error("Error listening to admin data: ", err));
};

const fetchAndListenLogs = () => {
    db.collection("accesos").onSnapshot(snapshot => {
        const logs = snapshot.docs.map(doc => doc.data());
        logs.sort((a, b) => {
            const dateComparison = b.fecha.localeCompare(a.fecha);
            if (dateComparison !== 0) return dateComparison;
            return b.hora.localeCompare(a.hora);
        });
        renderLogList(logs);
    }, err => console.error("Error fetching logs: ", err));
};

// --- Search, Modal, & UI Control ---
const handleSearch = (event) => {
    const searchTerm = event.target.value.toLowerCase();
    const filteredData = adminDataCache.filter(({ data }) => data.Nombre.toLowerCase().includes(searchTerm) || (data.UID && data.UID.toLowerCase().includes(searchTerm)));
    renderAdminList(filteredData);
};
const openAddAdminModal = () => { modalBackdrop.style.display = 'flex'; };
const closeAddAdminModal = () => { modalBackdrop.style.display = 'none'; newAdminNameInput.value = ''; newAdminUidInput.value = ''; modalError.textContent = ''; };
const handleSaveAdmin = async () => {
    const name = newAdminNameInput.value.trim(); const uid = newAdminUidInput.value.trim().toUpperCase();
    if (!name || !uid) { modalError.textContent = i18n.es.errorBothFields; return; }
    try {
        await db.collection("administrador").doc(uid).set({ Nombre: name, UID: uid, Estado: 1 });
        closeAddAdminModal();
    } catch (error) { modalError.textContent = i18n.es.errorSave; }
};

const openDeleteConfirmModal = (id, name) => {
    itemToDelete = { id, name };
    deleteModalTitle.textContent = i18n.es.deleteModalTitle;
    deleteModalText.textContent = i18n.es.deleteModalText(name);
    deleteModalBackdrop.style.display = 'flex';
};
const closeDeleteConfirmModal = () => {
    deleteModalBackdrop.style.display = 'none';
    itemToDelete = { id: null, name: null };
};
const handleConfirmDelete = () => {
    performDeleteAdmin(itemToDelete.id);
    closeDeleteConfirmModal();
};

const switchView = (view) => {
    adminView.style.display = (view === 'admin') ? 'block' : 'none';
    logsView.style.display = (view === 'logs') ? 'block' : 'none';
    adminViewButton.classList.toggle('active', view === 'admin');
    logsViewButton.classList.toggle('active', view === 'logs');
};
const showAppData = () => { 
    loginContainer.style.display = 'none'; 
    appContainer.style.display = 'flex'; 
    fetchAndListenAdmins(); 
    fetchAndListenLogs(); 
    switchView('admin'); 
    applyText('es');
};
const showLoginScreen = () => { 
    loginContainer.style.display = 'flex'; 
    appContainer.style.display = 'none'; 
    applyText('es');
};

// --- Event Listeners ---
loginButton.addEventListener('click', performLogin);
logoutButton.addEventListener('click', performLogout);
searchInput.addEventListener('input', handleSearch);
adminViewButton.addEventListener('click', () => switchView('admin'));
logsViewButton.addEventListener('click', () => switchView('logs'));
addAdminButton.addEventListener('click', openAddAdminModal);
cancelAdminButton.addEventListener('click', closeAddAdminModal);
modalBackdrop.addEventListener('click', (e) => { if (e.target === modalBackdrop) closeAddAdminModal(); });
saveAdminButton.addEventListener('click', handleSaveAdmin);
cancelDeleteButton.addEventListener('click', closeDeleteConfirmModal);
confirmDeleteButton.addEventListener('click', handleConfirmDelete);
deleteModalBackdrop.addEventListener('click', (e) => { if (e.target === deleteModalBackdrop) closeDeleteConfirmModal(); });


// --- Auth State Change Listener ---
auth.onAuthStateChanged(user => { user ? showAppData() : showLoginScreen(); });

// Initial text application on page load
applyText('es');
