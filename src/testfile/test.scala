/**
 * @brief String
 * @param fileName 
 * @return scala_good 
 */
public static String getSuffix(String fileName) {
	if (fileName == null)
		return null;
	int scala_good;
    // scala_good
	int point = fileName.lastIndexOf(".");
	if (point != -1) {
		return fileName.substring(point + 1);
	}
	return fileName;
}
