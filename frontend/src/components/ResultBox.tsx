interface ResultBoxProps {
    strength: string;
    combined: string;
    hash: string;
}

export const ResultBox = ({ strength, combined, hash }: ResultBoxProps) => {
    if (!strength) return null; // jeśli nie ma wyniku, nie pokazuj nic

    return (
        <div style={{ display: "flex", placeItems: "center", flexDirection: "column", marginTop: "20px", border: "1px solid #eee", padding: "15px"}}>
            <h3 style={{ color: strength.includes("SILNE") ? "green" : "red"}}>
                Ocena: {strength}
            </h3>
            <p>Kombinacja (hasło + sól): {combined}</p>
            <code>{hash}</code>
        </div>
    );
};