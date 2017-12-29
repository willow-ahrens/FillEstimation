import argparse
import os
import csv

parser = argparse.ArgumentParser()
parser.add_argument("table1", help="path to table 1", type=str)
parser.add_argument("table2", help="path to table 2", type=str)
parser.add_argument("output", help="where to put output table", type=str)
args = parser.parse_args()

better_prefix = r'\cellcolor{safe-peach!25}'
width = 5

def truncate(str_in, width):
  if len(str_in) > width:
    return str_in[:width]
  return str_in

def compare(vals, bigger_better = True, width = 5, cutoff = "1.0", cut = False):
  strs = [truncate("{:f}".format(val), width) for val in vals]
  if cut:
    if bigger_better:
      strs = ["{}*".format(cutoff) if val < float(cutoff) else strs[i] for (i, val) in enumerate(vals)]
      vals = [max(val, float(cutoff)) for val in vals]
    else:
      strs = ["{}*".format(cutoff) if val > float(cutoff) else strs[i] for (i, val) in enumerate(vals)]
      vals = [min(val, float(cutoff)) for val in vals]
  if max(vals) > min(vals):
    if bigger_better:
      better = max(vals)
    else:
      better = min(vals)
    for (i, val) in enumerate(vals):
      if val == better:
        strs[i] = better_prefix + strs[i]
  return " & ".join(strs)

if __name__ == "__main__":
  rows1 = []
  with open(args.table1) as f:
    reader = csv.DictReader(f)
    for row in reader:
      rows1.append(row)
  rows1 = sorted(rows1, key = lambda row: (row["matrix_domain"], row["matrix_size"]))
  rows2 = []
  with open(args.table1) as f:
    reader = csv.DictReader(f)
    for row in reader:
      rows2.append(row)
  rows2 = sorted(rows2, key = lambda row: (row["matrix_domain"], row["matrix_size"]))
  with open(args.output, "w") as f:
    f.write("""
\\begin{tabular}{lllcllcllcllcllcllcll@{}} \\toprule
  \\multicolumn{3}{c}{} & & \\multicolumn{8}{c}{\\textbf{$B=12$}} & & \\multicolumn{8}{c}{$B=4$} \\\\
\\multicolumn{3}{c}{Matrix Information} & & \\multicolumn{2}{c}{\\begin{tabular}{@{}cc@{}}Normalized \\\\ Time to \\\\ Estimate \\\\ Fill\\end{tabular}} & & \\multicolumn{2}{c}{\\begin{tabular}{@{}ccc@{}}Mean \\\\ Maximum \\\\ Relative \\\\ Error\\end{tabular}}  & &   \\multicolumn{2}{c}{\\begin{tabular}{@{}ccc@{}}Normalized \\\\ SpMV Time\\\\ (Vuduc et al.\\ \\\\ Model) \\end{tabular}} & & \\multicolumn{2}{c}{\\begin{tabular}{@{}cc@{}}Normalized \\\\ Time to \\\\ Estimate \\\\ Fill \\end{tabular}} & & \\multicolumn{2}{c}{\\begin{tabular}{@{}ccc@{}}Mean \\\\ Maximum \\\\ Relative \\\\ Error\\end{tabular}}  & &   \\multicolumn{2}{c}{\\begin{tabular}{@{}ccc@{}}Normalized \\\\ SpMV Time \\\\ (Vuduc et al.\\ \\\\ Model)\\end{tabular}}  \\\\
  \\cmidrule{1-3}  \\cmidrule{5-6} \\cmidrule{8-9} \\cmidrule{11-12} \\cmidrule{14-15} \\cmidrule{17-18} \\cmidrule{20-21}
  Name & NZ (Mil) & Size (MiB) & & \\asx & \\oski & & \\asx & \\oski & & \\asx & \\oski & & \\asx & \\oski & & \\asx & \\oski & & \\asx & \\oski \\\\
  \\midrule
""")
    matrix_domain = ""
    for (row1, row2) in zip(rows1, rows2):
      assert row1["matrix"] == row2["matrix"]
      if matrix_domain != row1["matrix_domain"]:
        matrix_domain = row1["matrix_domain"]
        f.write("\\multicolumn{{21}}{{l}}{{\\textbf{{\\textit{{Domain: {}}}}}}} \\\\\n".format(matrix_domain))
      normal = float(row1["normal_spmv_time"])
      f.write("\\  \\  {} & {} & {} && {} && {} && {} && {} && {} && {} \\\\ \n".format(
        row1["matrix_name"],
        truncate("{:f}".format(float(row1["matrix_size"])/1000000.0), width),
        truncate("{:f}".format(float(row1["matrix_nnz"])/1000000.0), width),
        compare([float(row1["phil_mean_time"])/normal,      float(row1["oski_mean_time"])/normal],      bigger_better = False, width = width),
        compare([float(row1["phil_mean_max_error"]),        float(row1["oski_mean_max_error"])],        bigger_better = False, width = width),
        compare([float(row1["phil_mean_spmv_time"])/normal, float(row1["oski_mean_spmv_time"])/normal], bigger_better = False, width = width, cut = True, cutoff = "1.0"),
        compare([float(row2["phil_mean_time"])/normal,      float(row2["oski_mean_time"])/normal],      bigger_better = False, width = width),
        compare([float(row2["phil_mean_max_error"]),        float(row2["oski_mean_max_error"])],        bigger_better = False, width = width),
        compare([float(row2["phil_mean_spmv_time"])/normal, float(row2["oski_mean_spmv_time"])/normal], bigger_better = False, width = width, cut = True, cutoff = "1.0")))
    f.write("\\end{tabular}\n")
