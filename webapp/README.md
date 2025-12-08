```
appweb-project/
├── .env.local             # Variables d'environnement (Clés Supabase)
├── next.config.js
├── package.json
├── README.md
├── public/                # Assets statiques (logos, images)
│
├── app/                   # Répertoire Next.js App Router
│   ├── layout.tsx         # Mise en page principale (HTML, Tailwind CSS, etc.)
│   ├── page.tsx           # Page d'accueil : Gère la redirection Auth/Dashboard
│   └── globals.css        # Styles globaux (inclut Tailwind)
│
├── components/            # Composants React réutilisables
│   ├── AuthForm.tsx       # Formulaire de connexion/inscription
│   ├── MainDashboard.tsx  # Layout principal après connexion
│   ├── MemoryTimeline.tsx # Le composant central de la ligne d'adresses
│   └── AnalysisForm.tsx   # Formulaire de création/édition d'analyse
│
├── lib/                   # Fonctions utilitaires, initialisation des clients
│   ├── supabase.ts        # Initialisation du client Supabase côté client (public)
│   └── supabase-server.ts # Initialisation du client Supabase côté serveur (sécurisé)
│
└── types/                 # Définitions de types TypeScript
    └── index.ts           # Interface Analysis, etc.
```