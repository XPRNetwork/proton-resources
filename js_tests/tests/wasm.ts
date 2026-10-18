import fs from "fs";
import { Blockchain } from "@proton/vert";

const EXPORT_SECTION = 7;
const MEMORY_KIND = 0x02;

const readLEB = (buf: Buffer, pos: number): [number, number] => {
  let result = 0;
  let shift = 0;
  let byte: number;
  do {
    byte = buf[pos++];
    result |= (byte & 0x7f) << shift;
    shift += 7;
  } while (byte & 0x80);
  return [result >>> 0, pos];
};

const writeLEB = (value: number): number[] => {
  const out: number[] = [];
  do {
    let byte = value & 0x7f;
    value >>>= 7;
    if (value) byte |= 0x80;
    out.push(byte);
  } while (value);
  return out;
};

// eosio.cdt 1.7 does not export linear memory, but VeRT reads it through `exports.memory`.
export const exportMemory = (wasm: Buffer): Buffer => {
  const module = new WebAssembly.Module(wasm);
  if (WebAssembly.Module.exports(module).some((e) => e.kind === "memory")) return wasm;

  const parts: Buffer[] = [wasm.subarray(0, 8)];
  let patched = false;
  let pos = 8;
  while (pos < wasm.length) {
    const id = wasm[pos];
    const [size, bodyStart] = readLEB(wasm, pos + 1);
    const end = bodyStart + size;

    if (id === EXPORT_SECTION) {
      const [count, entriesStart] = readLEB(wasm, bodyStart);
      const name = Buffer.from("memory");
      const body = Buffer.concat([
        Buffer.from(writeLEB(count + 1)),
        wasm.subarray(entriesStart, end),
        Buffer.from([...writeLEB(name.length), ...name, MEMORY_KIND, 0]),
      ]);
      parts.push(Buffer.from([EXPORT_SECTION, ...writeLEB(body.length)]), body);
      patched = true;
    } else {
      parts.push(wasm.subarray(pos, end));
    }
    pos = end;
  }

  if (!patched) throw new Error("wasm has no export section");
  return Buffer.concat(parts);
};

export const createCdtContract = (blockchain: Blockchain, name: string, path: string) =>
  blockchain.createAccount({
    name,
    wasm: exportMemory(fs.readFileSync(`${path}.wasm`)),
    abi: fs.readFileSync(`${path}.abi`, "utf8"),
    enableInline: true,
  });
