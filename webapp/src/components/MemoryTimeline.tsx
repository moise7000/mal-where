'use client';

import { useMemo, useState, useRef, useEffect } from 'react';
import { Analysis } from '@/types';

interface MemoryTimelineProps {
    analyses: Analysis[];
    onRangeSelect: (start: number, end: number) => void;
}

const BASE_ADDRESS = 0x00400000;
const MAX_ADDRESS = 0x00410000;
const TOTAL_SIZE = MAX_ADDRESS - BASE_ADDRESS;

export default function MemoryTimeline({ analyses, onRangeSelect }: MemoryTimelineProps) {
    const containerRef = useRef<HTMLDivElement>(null);
    const [hoverAddress, setHoverAddress] = useState<number | null>(null);
    const [hoverX, setHoverX] = useState<number>(0);
    const [selectionStart, setSelectionStart] = useState<number | null>(null);

    // Calculer les blocs existants (Zone Bleue)
    const AnalysisBlocks = useMemo(() => {
        return analyses.map((analysis) => {
            const start = analysis.start_address;
            const end = analysis.end_address;
            if (end <= BASE_ADDRESS || start >= MAX_ADDRESS) return null;

            const offset = Math.max(0, start - BASE_ADDRESS);
            const length = Math.min(MAX_ADDRESS, end) - Math.max(BASE_ADDRESS, start);
            const leftPercent = (offset / TOTAL_SIZE) * 100;
            const widthPercent = (length / TOTAL_SIZE) * 100;

            return (
                <div
                    key={analysis.id}
                    className="absolute h-full bg-blue-500/80 border-x border-blue-700 hover:bg-blue-600 transition-colors z-10 cursor-pointer group"
                    style={{ left: `${leftPercent}%`, width: `${widthPercent}%` }}
                    onClick={(e) => {
                        e.stopPropagation();
                        alert(`📂 Document: ${analysis.title}`);
                    }}
                >
                    {/* Tooltip au survol d'un bloc existant */}
                    <div className="hidden group-hover:block absolute bottom-full left-1/2 -translate-x-1/2 mb-2 px-2 py-1 bg-gray-900 text-white text-xs rounded whitespace-nowrap z-50 pointer-events-none">
                        {analysis.title}
                    </div>
                </div>
            );
        });
    }, [analyses]);

    // Gestion du mouvement de souris (Calcul adresse + position)
    const handleMouseMove = (e: React.MouseEvent<HTMLDivElement>) => {
        if (!containerRef.current) return;

        const rect = containerRef.current.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const percentage = Math.max(0, Math.min(1, x / rect.width)); // Clamp entre 0 et 1

        // Calcul adresse (arrondi 16 bytes)
        const rawAddress = BASE_ADDRESS + (TOTAL_SIZE * percentage);
        const address = Math.floor(rawAddress / 16) * 16;

        setHoverAddress(address);
        setHoverX(percentage * 100);
    };

    const handleMouseLeave = () => {
        setHoverAddress(null);
    };

    const handleClick = () => {
        if (hoverAddress === null) return;

        if (selectionStart === null) {
            // Premier clic : on fige le début
            setSelectionStart(hoverAddress);
        } else {
            // Deuxième clic : on valide la fin
            const start = Math.min(selectionStart, hoverAddress);
            const end = Math.max(selectionStart, hoverAddress);

            if (end > start) {
                onRangeSelect(start, end);
                setSelectionStart(null); // Reset après sélection
            } else {
                // Si l'utilisateur clique au même endroit, on annule
                setSelectionStart(null);
            }
        }
    };

    // Calcul de la zone de prévisualisation (Zone Grise hachurée) pendant la sélection
    const selectionPreviewStyle = useMemo(() => {
        if (selectionStart === null || hoverAddress === null) return null;

        const start = Math.min(selectionStart, hoverAddress);
        const end = Math.max(selectionStart, hoverAddress);
        const offset = start - BASE_ADDRESS;
        const length = end - start;

        return {
            left: `${(offset / TOTAL_SIZE) * 100}%`,
            width: `${(length / TOTAL_SIZE) * 100}%`
        };
    }, [selectionStart, hoverAddress]);

    return (
        <div className="w-full select-none">
            {/* Header avec infos */}
            <div className="flex justify-between items-end mb-2 px-1">
                <span className="font-mono text-xs text-gray-500">0x{BASE_ADDRESS.toString(16).toUpperCase()}</span>

                {/* Indicateur central dynamique */}
                <div className="font-mono text-sm font-bold text-blue-700 bg-blue-50 px-2 py-1 rounded">
                    {hoverAddress
                        ? `0x${hoverAddress.toString(16).toUpperCase()}`
                        : "Survolez la mémoire"}
                </div>

                <span className="font-mono text-xs text-gray-500">0x{MAX_ADDRESS.toString(16).toUpperCase()}</span>
            </div>

            {/* La Barre Mémoire */}
            <div
                ref={containerRef}
                className="relative w-full h-16 bg-gray-200 border-2 border-gray-300 rounded cursor-crosshair overflow-hidden shadow-inner"
                onMouseMove={handleMouseMove}
                onMouseLeave={handleMouseLeave}
                onClick={handleClick}
            >
                {/* 1. Blocs existants */}
                {AnalysisBlocks}

                {/* 2. Zone de sélection en cours (Fantôme) */}
                {selectionPreviewStyle && (
                    <div
                        className="absolute h-full bg-green-500/30 border-x-2 border-green-600 z-20 pointer-events-none"
                        style={selectionPreviewStyle}
                    />
                )}

                {/* 3. Ligne rouge (Curseur ou Point de départ) */}
                {(hoverAddress !== null || selectionStart !== null) && (
                    <div
                        className="absolute top-0 bottom-0 border-l-2 border-red-500 z-30 pointer-events-none transition-all duration-75"
                        style={{
                            left: selectionStart !== null && hoverAddress === null
                                ? `${((selectionStart - BASE_ADDRESS) / TOTAL_SIZE) * 100}%`
                                : `${hoverX}%`
                        }}
                    >
                        {/* Petit flag "Start" si on a commencé à sélectionner */}
                        {selectionStart !== null && (
                            <span className="absolute top-0 left-0 -translate-y-full bg-red-600 text-white text-[10px] px-1 rounded-t">
                                START
                            </span>
                        )}
                    </div>
                )}
            </div>

            {/* Instructions contextuelles */}
            <p className="mt-2 text-center text-sm text-gray-500">
                {selectionStart === null
                    ? "🖱️ Cliquez pour définir le point de DÉBUT"
                    : "🖱️ Déplacez et cliquez pour définir la FIN"}
            </p>
        </div>
    );
}