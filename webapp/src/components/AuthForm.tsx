// components/AuthForm.tsx (Client Component)
'use client';

import { useState } from 'react';
import { supabase } from '@/lib/supabase';

export default function AuthForm() {
    const [isSigningUp, setIsSigningUp] = useState(false);
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');
    const [loading, setLoading] = useState(false);

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        setLoading(true);

        let authPromise;
        if (isSigningUp) {
            authPromise = supabase.auth.signUp({ email, password });
        } else {
            authPromise = supabase.auth.signInWithPassword({ email, password });
        }

        const { error } = await authPromise;

        if (error) {
            alert(`Erreur d'authentification: ${error.message}`);
        } else {
            alert(isSigningUp ? 'Vérifiez votre email pour confirmer l\'inscription.' : 'Connexion réussie !');
            // Supabase gère l'état de session, le rechargement/redirection est souvent automatique
        }
        setLoading(false);
    };

    return (
        <div className="max-w-md mx-auto mt-10 p-6 bg-white shadow-lg rounded-lg">
            <h2 className="text-2xl font-bold mb-6 text-center text-gray-800">
                {isSigningUp ? 'Créer un Compte' : 'Connexion'}
            </h2>
            <form onSubmit={handleSubmit} className="space-y-4">
                <div>
                    <label htmlFor="email" className="block text-sm font-medium text-gray-700">Email</label>
                    <input
                        id="email"
                        type="email"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        required
                        className="mt-1 block w-full px-3 py-2 border border-gray-300 rounded-md shadow-sm focus:outline-none focus:ring-blue-500 focus:border-blue-500"
                    />
                </div>
                <div>
                    <label htmlFor="password" className="block text-sm font-medium text-gray-700">Mot de passe</label>
                    <input
                        id="password"
                        type="password"
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                        required
                        className="mt-1 block w-full px-3 py-2 border border-gray-300 rounded-md shadow-sm focus:outline-none focus:ring-blue-500 focus:border-blue-500"
                    />
                </div>
                <button
                    type="submit"
                    disabled={loading}
                    className="w-full flex justify-center py-2 px-4 border border-transparent rounded-md shadow-sm text-sm font-medium text-white bg-blue-600 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500 disabled:bg-blue-400"
                >
                    {loading ? 'Chargement...' : isSigningUp ? 'S\'inscrire' : 'Se Connecter'}
                </button>
            </form>
            <button
                onClick={() => setIsSigningUp(!isSigningUp)}
                className="mt-4 w-full text-sm text-blue-600 hover:text-blue-800"
            >
                {isSigningUp ? 'J\'ai déjà un compte' : 'Besoin d\'un compte? S\'inscrire'}
            </button>
        </div>
    );
}