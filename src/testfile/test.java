/**
 * @brief String
 * @param fileName 
 * @return good 
 */
public static String getSuffix(String fileName) {
	if (fileName == null)
		return null;
	int good;
	int point = fileName.lastIndexOf(".");
	if (point != -1) {
		return fileName.substring(point + 1);
	}
	return fileName;
}
