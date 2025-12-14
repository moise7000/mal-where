'use client';
import { useState } from 'react';
import { Analysis } from '@/types';

interface AnalysisFormProps {
    startAddress: number;
    endAddress: number;
    onAnalysisCreated: (analysis: Analysis) => void;
}

export default function AnalysisForm({ startAddress, endAddress, onAnalysisCreated }: AnalysisFormProps) {
    const [title, setTitle] = useState('');
    const [description, setDescription] = useState('');

    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
        const newAnalysis: Analysis = {
            id: Math.random().toString(36).substr(2, 9),
            user_id: 'local',
            start_address: startAddress,
            end_address: endAddress,
            title,
            description
        };
        onAnalysisCreated(newAnalysis);
    };

    return (
        <form onSubmit={handleSubmit} className="space-y-4 animate-in fade-in slide-in-from-top-4">
            <div className="flex gap-4 text-sm font-mono text-blue-700 bg-blue-50 p-2 rounded">
                <span>Start: 0x{startAddress.toString(16)}</span>
                <span>End: 0x{endAddress.toString(16)}</span>
            </div>
            <input
                type="text" placeholder="Titre de la zone" value={title} onChange={e => setTitle(e.target.value)}
                required className="w-full p-2 border rounded"
            />
            <textarea
                placeholder="Description..." value={description} onChange={e => setDescription(e.target.value)}
                className="w-full p-2 border rounded" rows={3}
            />
            <button type="submit" className="w-full bg-blue-600 text-white py-2 rounded hover:bg-blue-700">Enregistrer</button>
        </form>
    );
}