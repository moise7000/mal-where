export interface Index {
    id: string;
    user_id: string;
    start_address: number;
    end_address: number;
    title: string;
    description: string;
    pdf_path?: string;
    created_at?: string;
}