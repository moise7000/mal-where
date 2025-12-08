'use client';

import { useState, useRef, useMemo } from 'react';

interface Analysis {
    id: string;
    user_id: string;
    start_address: number;
    end_address: number;
    title: string;
    description: string;
    color?: string;
}

const COLORS = [
    'bg-blue-500', 'bg-purple-500', 'bg-pink-500',
    'bg-green-500', 'bg-yellow-500', 'bg-red-500'
];

const BASE_ADDRESS = 0x00400000;
const MAX_ADDRESS = 0x00410000;
const TOTAL_SIZE = MAX_ADDRESS - BASE_ADDRESS;

export default function MainDashboard() {
    const [analyses, setAnalyses] = useState<Analysis[]>([
        {
            id: '1',
            user_id: 'demo',
            start_address: 0x00401000,
            end_address: 0x00401500,
            title: 'PE Header',
            description: 'En-tête standard du fichier PE',
            color: 'bg-blue-500'
        },
        {
            id: '2',
            user_id: 'demo',
            start_address: 0x00405000,
            end_address: 0x00406000,
            title: '.text Section',
            description: 'Code exécutable principal',
            color: 'bg-purple-500'
        }
    ]);

    const [selectionStart, setSelectionStart] = useState<number | null>(null);
    const [hoverAddress, setHoverAddress] = useState<number | null>(null);
    const [showForm, setShowForm] = useState(false);
    const [formData, setFormData] = useState({ title: '', description: '' });
    const [selectedAnalysis, setSelectedAnalysis] = useState<string | null>(null);

    const timelineRef = useRef<HTMLDivElement>(null);

    const handleTimelineInteraction = (e: React.MouseEvent<HTMLDivElement>, isClick: boolean) => {
        if (!timelineRef.current) return;

        const rect = timelineRef.current.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const percentage = Math.max(0, Math.min(1, x / rect.width));
        const rawAddress = BASE_ADDRESS + (TOTAL_SIZE * percentage);
        const address = Math.floor(rawAddress / 16) * 16;

        if (isClick) {
            if (selectionStart === null) {
                setSelectionStart(address);
                setShowForm(false);
            } else {
                const start = Math.min(selectionStart, address);
                const end = Math.max(selectionStart, address);
                if (end > start) {
                    setShowForm(true);
                }
            }
        } else {
            setHoverAddress(address);
        }
    };

    const handleSubmit = () => {
        if (selectionStart === null || hoverAddress === null) return;
        if (!formData.title.trim()) return;

        const start = Math.min(selectionStart, hoverAddress);
        const end = Math.max(selectionStart, hoverAddress);

        const newAnalysis: Analysis = {
            id: Math.random().toString(36).substr(2, 9),
            user_id: 'local',
            start_address: start,
            end_address: end,
            title: formData.title,
            description: formData.description,
            color: COLORS[analyses.length % COLORS.length]
        };

        setAnalyses([...analyses, newAnalysis]);
        setSelectionStart(null);
        setShowForm(false);
        setFormData({ title: '', description: '' });
    };

    const cancelSelection = () => {
        setSelectionStart(null);
        setShowForm(false);
        setFormData({ title: '', description: '' });
    };

    const analysisBlocks = useMemo(() => {
        return analyses.map((analysis) => {
            const offset = Math.max(0, analysis.start_address - BASE_ADDRESS);
            const length = Math.min(MAX_ADDRESS, analysis.end_address) - Math.max(BASE_ADDRESS, analysis.start_address);
            const leftPercent = (offset / TOTAL_SIZE) * 100;
            const widthPercent = (length / TOTAL_SIZE) * 100;

            return { ...analysis, leftPercent, widthPercent };
        });
    }, [analyses]);

    const selectionPreview = useMemo(() => {
        if (selectionStart === null || hoverAddress === null) return null;

        const start = Math.min(selectionStart, hoverAddress);
        const end = Math.max(selectionStart, hoverAddress);
        const offset = start - BASE_ADDRESS;
        const length = end - start;

        return {
            left: (offset / TOTAL_SIZE) * 100,
            width: (length / TOTAL_SIZE) * 100
        };
    }, [selectionStart, hoverAddress]);

    return (
        <div className="min-h-screen bg-gradient-to-br from-slate-50 to-slate-100">
            {/* Header */}
            <header className="bg-white border-b shadow-sm">
                <div className="max-w-7xl mx-auto px-6 py-6">
                    <div className="flex items-center gap-3">
                        <div className="w-10 h-10 bg-gradient-to-br from-blue-500 to-purple-600 rounded-lg flex items-center justify-center">
                            <span className="text-white text-xl">🔬</span>
                        </div>
                        <div>
                            <h1 className="text-2xl font-bold text-slate-800">ASM Reverse Engineering</h1>
                            <p className="text-sm text-slate-500">Cartographie interactive de la mémoire</p>
                        </div>
                    </div>
                </div>
            </header>

            <div className="max-w-7xl mx-auto px-6 py-8">
                {/* Timeline principale */}
                <div className="bg-white rounded-2xl shadow-lg p-8 mb-8">
                    <div className="mb-6">
                        <h2 className="text-lg font-semibold text-slate-800 mb-2">Espace mémoire</h2>
                        <p className="text-sm text-slate-500">
                            {selectionStart === null
                                ? "Cliquez pour définir le début de la zone"
                                : showForm
                                    ? "Remplissez le formulaire ci-dessous"
                                    : "Cliquez à nouveau pour définir la fin"}
                        </p>
                    </div>

                    {/* Adresses de référence */}
                    <div className="flex justify-between mb-3 px-2">
            <span className="font-mono text-xs font-semibold text-slate-600">
              0x{BASE_ADDRESS.toString(16).toUpperCase()}
            </span>

                        {/* Adresse courante - Grande et centrale */}
                        <div className="flex flex-col items-center">
                            <span className="text-xs text-slate-400 mb-1">Adresse actuelle</span>
                            <div className="font-mono text-2xl font-bold bg-gradient-to-r from-blue-600 to-purple-600 bg-clip-text text-transparent">
                                {hoverAddress ? `0x${hoverAddress.toString(16).toUpperCase()}` : '—'}
                            </div>
                        </div>

                        <span className="font-mono text-xs font-semibold text-slate-600">
              0x{MAX_ADDRESS.toString(16).toUpperCase()}
            </span>
                    </div>

                    {/* La barre de timeline */}
                    <div className="relative">
                        {/* Graduations */}
                        <div className="absolute -top-2 left-0 right-0 flex justify-between px-2">
                            {[0, 0.25, 0.5, 0.75, 1].map((pos) => (
                                <div key={pos} className="flex flex-col items-center">
                                    <div className="w-px h-2 bg-slate-300"></div>
                                </div>
                            ))}
                        </div>

                        {/* Timeline interactive */}
                        <div
                            ref={timelineRef}
                            className="relative h-24 bg-gradient-to-r from-slate-100 via-slate-50 to-slate-100 rounded-xl border-2 border-slate-200 cursor-crosshair overflow-hidden shadow-inner"
                            onMouseMove={(e) => handleTimelineInteraction(e, false)}
                            onMouseLeave={() => setHoverAddress(null)}
                            onClick={(e) => handleTimelineInteraction(e, true)}
                        >
                            {/* Blocs existants */}
                            {analysisBlocks.map((block) => (
                                <div
                                    key={block.id}
                                    className={`absolute h-full ${block.color} opacity-80 hover:opacity-100 transition-all cursor-pointer group border-x-2 border-white`}
                                    style={{ left: `${block.leftPercent}%`, width: `${block.widthPercent}%` }}
                                    onClick={(e) => {
                                        e.stopPropagation();
                                        setSelectedAnalysis(block.id === selectedAnalysis ? null : block.id);
                                    }}
                                >
                                    <div className="h-full flex items-center justify-center px-2">
                    <span className="text-white text-xs font-semibold truncate opacity-0 group-hover:opacity-100 transition-opacity">
                      {block.title}
                    </span>
                                    </div>

                                    {/* Tooltip */}
                                    <div className="absolute bottom-full left-1/2 -translate-x-1/2 mb-2 px-3 py-1.5 bg-slate-900 text-white text-xs rounded-lg opacity-0 group-hover:opacity-100 transition-opacity pointer-events-none whitespace-nowrap z-50 shadow-lg">
                                        <div className="font-semibold">{block.title}</div>
                                        <div className="text-slate-300 font-mono text-[10px]">
                                            0x{block.start_address.toString(16).toUpperCase()} - 0x{block.end_address.toString(16).toUpperCase()}
                                        </div>
                                    </div>
                                </div>
                            ))}

                            {/* Zone de sélection en cours */}
                            {selectionPreview && !showForm && (
                                <div
                                    className="absolute h-full bg-emerald-400 opacity-40 border-2 border-emerald-500 pointer-events-none animate-pulse"
                                    style={{ left: `${selectionPreview.left}%`, width: `${selectionPreview.width}%` }}
                                />
                            )}

                            {/* Curseur/Ligne de sélection */}
                            {(hoverAddress !== null || selectionStart !== null) && (
                                <div
                                    className="absolute top-0 bottom-0 w-0.5 bg-red-500 pointer-events-none z-50"
                                    style={{
                                        left: selectionStart !== null && hoverAddress === null
                                            ? `${((selectionStart - BASE_ADDRESS) / TOTAL_SIZE) * 100}%`
                                            : hoverAddress !== null
                                                ? `${((hoverAddress - BASE_ADDRESS) / TOTAL_SIZE) * 100}%`
                                                : '0%'
                                    }}
                                >
                                    <div className="absolute top-0 left-1/2 -translate-x-1/2 -translate-y-full">
                                        <div className="bg-red-500 text-white text-[10px] px-2 py-0.5 rounded-t font-mono whitespace-nowrap">
                                            {selectionStart !== null && hoverAddress === null ? 'DÉBUT' : 'CURSEUR'}
                                        </div>
                                    </div>
                                </div>
                            )}
                        </div>

                        {/* Info de sélection */}
                        {selectionStart !== null && hoverAddress !== null && !showForm && (
                            <div className="mt-4 flex items-center justify-center gap-4 text-sm">
                                <div className="font-mono bg-emerald-50 text-emerald-700 px-3 py-2 rounded-lg border border-emerald-200">
                                    <span className="text-xs text-emerald-600">Début: </span>
                                    <span className="font-bold">0x{Math.min(selectionStart, hoverAddress).toString(16).toUpperCase()}</span>
                                </div>
                                <span className="text-slate-400">→</span>
                                <div className="font-mono bg-emerald-50 text-emerald-700 px-3 py-2 rounded-lg border border-emerald-200">
                                    <span className="text-xs text-emerald-600">Fin: </span>
                                    <span className="font-bold">0x{Math.max(selectionStart, hoverAddress).toString(16).toUpperCase()}</span>
                                </div>
                                <button
                                    onClick={cancelSelection}
                                    className="ml-4 text-red-600 hover:text-red-700 text-sm font-medium"
                                >
                                    ✕ Annuler
                                </button>
                            </div>
                        )}
                    </div>
                </div>

                {/* Formulaire de création */}
                {showForm && selectionStart !== null && hoverAddress !== null && (
                    <div className="bg-white rounded-2xl shadow-lg p-8 mb-8 border-2 border-emerald-200 animate-in slide-in-from-top-4">
                        <div className="flex justify-between items-start mb-6">
                            <div>
                                <h3 className="text-xl font-bold text-slate-800 mb-2">Nouvelle analyse</h3>
                                <div className="flex gap-3 text-sm">
                  <span className="font-mono bg-slate-100 text-slate-700 px-3 py-1 rounded">
                    0x{Math.min(selectionStart, hoverAddress).toString(16).toUpperCase()}
                  </span>
                                    <span className="text-slate-400">→</span>
                                    <span className="font-mono bg-slate-100 text-slate-700 px-3 py-1 rounded">
                    0x{Math.max(selectionStart, hoverAddress).toString(16).toUpperCase()}
                  </span>
                                </div>
                            </div>
                            <button
                                onClick={cancelSelection}
                                className="text-slate-400 hover:text-red-500 transition-colors"
                            >
                                <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                                </svg>
                            </button>
                        </div>

                        <div className="space-y-4">
                            <div>
                                <label className="block text-sm font-medium text-slate-700 mb-2">
                                    Titre de la zone
                                </label>
                                <input
                                    type="text"
                                    value={formData.title}
                                    onChange={(e) => setFormData({ ...formData, title: e.target.value })}
                                    placeholder="Ex: PE Header, .text Section..."
                                    className="w-full px-4 py-2.5 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-transparent outline-none transition-all"
                                />
                            </div>

                            <div>
                                <label className="block text-sm font-medium text-slate-700 mb-2">
                                    Description
                                </label>
                                <textarea
                                    value={formData.description}
                                    onChange={(e) => setFormData({ ...formData, description: e.target.value })}
                                    placeholder="Détails de votre analyse..."
                                    rows={4}
                                    className="w-full px-4 py-2.5 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-transparent outline-none transition-all resize-none"
                                />
                            </div>

                            <div className="flex gap-3 pt-2">
                                <button
                                    onClick={handleSubmit}
                                    className="flex-1 bg-gradient-to-r from-blue-600 to-purple-600 text-white py-3 rounded-lg font-semibold hover:shadow-lg transition-all"
                                >
                                    Enregistrer l'analyse
                                </button>
                                <button
                                    onClick={cancelSelection}
                                    className="px-6 py-3 border border-slate-300 text-slate-700 rounded-lg font-semibold hover:bg-slate-50 transition-all"
                                >
                                    Annuler
                                </button>
                            </div>
                        </div>
                    </div>
                )}

                {/* Liste des analyses */}
                <div>
                    <h2 className="text-xl font-bold text-slate-800 mb-4">Analyses documentées</h2>
                    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
                        {analyses.map((analysis) => (
                            <div
                                key={analysis.id}
                                className={`bg-white rounded-xl p-5 border-2 transition-all cursor-pointer ${
                                    selectedAnalysis === analysis.id
                                        ? 'border-blue-500 shadow-lg'
                                        : 'border-slate-200 hover:border-slate-300 hover:shadow-md'
                                }`}
                                onClick={() => setSelectedAnalysis(analysis.id === selectedAnalysis ? null : analysis.id)}
                            >
                                <div className="flex items-start gap-3 mb-3">
                                    <div className={`w-3 h-3 rounded-full ${analysis.color} mt-1`}></div>
                                    <h3 className="font-bold text-slate-800 flex-1">{analysis.title}</h3>
                                </div>

                                <div className="font-mono text-xs bg-slate-100 text-slate-600 px-2 py-1.5 rounded mb-3 inline-block">
                                    0x{analysis.start_address.toString(16).toUpperCase()} - 0x{analysis.end_address.toString(16).toUpperCase()}
                                </div>

                                <p className="text-slate-600 text-sm leading-relaxed">
                                    {analysis.description}
                                </p>
                            </div>
                        ))}
                    </div>
                </div>
            </div>
        </div>
    );
}